BEGIN;

/* ======================
   基础主数据
====================== */
CREATE TABLE IF NOT EXISTS departments (
  department_id INTEGER PRIMARY KEY,
  name TEXT NOT NULL UNIQUE
);

CREATE TABLE IF NOT EXISTS patients (
  patient_id INTEGER PRIMARY KEY,
  username TEXT NOT NULL UNIQUE,            -- 与前端默认 "Jane" 对齐
  name TEXT NOT NULL,
  gender TEXT CHECK (gender IN ('男','女','其他')) DEFAULT '其他',
  birth_date TEXT,                          -- ISO8601: YYYY-MM-DD
  id_number TEXT,
  blood_type TEXT CHECK (blood_type IN ('A','B','AB','O','其他')) DEFAULT '其他',
  mobile TEXT,
  email TEXT,
  emergency_contact TEXT,
  emergency_phone TEXT,
  insurance_type TEXT,
  insurance_card TEXT,
  allergies TEXT,
  history TEXT,
  created_at TEXT NOT NULL DEFAULT (datetime('now'))
);

CREATE TABLE IF NOT EXISTS doctors (
  doctor_id INTEGER PRIMARY KEY,
  name TEXT NOT NULL,
  title TEXT NOT NULL CHECK (title IN ('主任医师','副主任医师','主治医师','住院医师','医师')),
  department_id INTEGER NOT NULL,
  specialty TEXT,
  experience TEXT,                 -- 资历/经历描述
  consultation_fee REAL NOT NULL DEFAULT 0.0,
  is_online INTEGER NOT NULL DEFAULT 0 CHECK (is_online IN (0,1)),
  FOREIGN KEY (department_id) REFERENCES departments(department_id) ON DELETE RESTRICT ON UPDATE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_doctors_dept ON doctors(department_id);
CREATE INDEX IF NOT EXISTS idx_doctors_name ON doctors(name);

/* ======================
   订单 / 支付（供 OnlinePaymentWidget）
====================== */
CREATE TABLE IF NOT EXISTS orders (
  order_id INTEGER PRIMARY KEY,
  patient_id INTEGER NOT NULL,
  department_id INTEGER,
  doctor_id INTEGER,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  discount REAL NOT NULL DEFAULT 0.0 CHECK (discount >= 0.0),
  total_amount REAL NOT NULL DEFAULT 0.0 CHECK (total_amount >= 0.0),
  status TEXT NOT NULL DEFAULT 'created' CHECK (status IN ('created','paid','canceled','refunded')),
  FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
  FOREIGN KEY (department_id) REFERENCES departments(department_id) ON DELETE SET NULL,
  FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE SET NULL
);
CREATE INDEX IF NOT EXISTS idx_orders_patient_created ON orders(patient_id, created_at DESC);

CREATE TABLE IF NOT EXISTS order_items (
  item_id INTEGER PRIMARY KEY,
  order_id INTEGER NOT NULL,
  item_name TEXT NOT NULL,                 -- 例如：挂号费/诊疗费/药费/检查费
  amount REAL NOT NULL CHECK (amount >= 0.0),
  FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id);

CREATE TABLE IF NOT EXISTS payments (
  payment_id INTEGER PRIMARY KEY,
  order_id INTEGER NOT NULL,
  method TEXT NOT NULL CHECK (method IN ('wechat','alipay')),
  amount REAL NOT NULL CHECK (amount >= 0.0),
  status TEXT NOT NULL DEFAULT 'pending' CHECK (status IN ('pending','success','failed')),
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  transaction_ref TEXT,
  FOREIGN KEY (order_id) REFERENCES orders(order_id) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_payments_order ON payments(order_id);

/* 自动汇总订单金额（sum(items)-discount，且不为负） */
CREATE TRIGGER IF NOT EXISTS trg_orders_recalc_after_insert_item
AFTER INSERT ON order_items
BEGIN
  UPDATE orders
  SET total_amount = MAX(0.0, (SELECT COALESCE(SUM(amount),0) FROM order_items WHERE order_id=NEW.order_id) - discount)
  WHERE order_id = NEW.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_recalc_after_update_item
AFTER UPDATE OF amount ON order_items
BEGIN
  UPDATE orders
  SET total_amount = MAX(0.0, (SELECT COALESCE(SUM(amount),0) FROM order_items WHERE order_id=NEW.order_id) - discount)
  WHERE order_id = NEW.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_recalc_after_delete_item
AFTER DELETE ON order_items
BEGIN
  UPDATE orders
  SET total_amount = MAX(0.0, (SELECT COALESCE(SUM(amount),0) FROM order_items WHERE order_id=OLD.order_id) - discount)
  WHERE order_id = OLD.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_recalc_after_update_discount
AFTER UPDATE OF discount ON orders
BEGIN
  UPDATE orders
  SET total_amount = MAX(0.0, (SELECT COALESCE(SUM(amount),0) FROM order_items WHERE order_id=NEW.order_id) - NEW.discount)
  WHERE order_id = NEW.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_after_insert
AFTER INSERT ON orders
BEGIN
  UPDATE orders
  SET total_amount = MAX(0.0, (SELECT COALESCE(SUM(amount),0) FROM order_items WHERE order_id=NEW.order_id) - discount)
  WHERE order_id = NEW.order_id;
END;

/* 支付成功后，若累计成功支付金额 >= 订单金额，则置订单为 paid */
CREATE TRIGGER IF NOT EXISTS trg_orders_mark_paid_after_insert_payment
AFTER INSERT ON payments
WHEN NEW.status = 'success'
BEGIN
  UPDATE orders
  SET status = CASE
      WHEN (SELECT COALESCE(SUM(amount),0) FROM payments WHERE order_id = NEW.order_id AND status='success')
           >= (SELECT total_amount FROM orders WHERE order_id = NEW.order_id)
      THEN 'paid' ELSE status END
  WHERE order_id = NEW.order_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_orders_mark_paid_after_update_payment
AFTER UPDATE OF status ON payments
WHEN NEW.status = 'success'
BEGIN
  UPDATE orders
  SET status = CASE
      WHEN (SELECT COALESCE(SUM(amount),0) FROM payments WHERE order_id = NEW.order_id AND status='success')
           >= (SELECT total_amount FROM orders WHERE order_id = NEW.order_id)
      THEN 'paid' ELSE status END
  WHERE order_id = NEW.order_id;
END;

/* ======================
   号源 & 预约（供 AppointmentBookingWidget / AppointmentDialog）
====================== */
CREATE TABLE IF NOT EXISTS appointment_slots (
  slot_id INTEGER PRIMARY KEY,
  doctor_id INTEGER NOT NULL,
  slot_start TEXT NOT NULL,     -- ISO8601: YYYY-MM-DD HH:MM:SS (建议UTC)
  slot_end   TEXT NOT NULL,
  status TEXT NOT NULL DEFAULT 'available' CHECK (status IN ('available','booked','closed')),
  fee REAL,                     -- 可覆盖医生基础诊疗费
  UNIQUE(doctor_id, slot_start),
  CHECK (strftime('%s', slot_end) > strftime('%s', slot_start)),
  FOREIGN KEY (doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_slots_doctor_start ON appointment_slots(doctor_id, slot_start);
CREATE INDEX IF NOT EXISTS idx_slots_status ON appointment_slots(status);

CREATE TABLE IF NOT EXISTS appointments (
  appointment_id INTEGER PRIMARY KEY,
  patient_id INTEGER NOT NULL,
  doctor_id  INTEGER NOT NULL,
  slot_id    INTEGER NOT NULL UNIQUE,       -- 每个时间段只允许一条预约
  status TEXT NOT NULL DEFAULT 'booked' CHECK (status IN ('booked','canceled','completed','no_show')),
  disease_description TEXT,
  fee REAL,                                 -- 预约时锁定费用
  order_id INTEGER,                         -- 可选，关联到支付订单
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
  FOREIGN KEY (doctor_id)  REFERENCES doctors(doctor_id) ON DELETE CASCADE,
  FOREIGN KEY (slot_id)    REFERENCES appointment_slots(slot_id) ON DELETE RESTRICT,
  FOREIGN KEY (order_id)   REFERENCES orders(order_id) ON DELETE SET NULL
);

/* 预约一致性与占位 */
CREATE TRIGGER IF NOT EXISTS trg_appt_before_insert
BEFORE INSERT ON appointments
BEGIN
  /* slot 必须存在且可用，且 slot 的医生要与 NEW.doctor_id 匹配 */
  SELECT CASE
    WHEN (SELECT status FROM appointment_slots WHERE slot_id=NEW.slot_id) IS NULL
      THEN RAISE(ABORT, '无效的slot_id')
    WHEN (SELECT status FROM appointment_slots WHERE slot_id=NEW.slot_id) <> 'available'
      THEN RAISE(ABORT, '该时间段不可用或已被预约')
    WHEN (SELECT doctor_id FROM appointment_slots WHERE slot_id=NEW.slot_id) <> NEW.doctor_id
      THEN RAISE(ABORT, 'slot与doctor_id不匹配')
  END;
END;

CREATE TRIGGER IF NOT EXISTS trg_appt_after_insert
AFTER INSERT ON appointments
BEGIN
  UPDATE appointment_slots SET status='booked' WHERE slot_id=NEW.slot_id;
  /* 若未显式写入fee，则从slot.fee→doctors.consultation_fee回填 */
  UPDATE appointments
  SET fee = COALESCE(NEW.fee,
                     (SELECT COALESCE(s.fee, d.consultation_fee)
                      FROM appointment_slots s JOIN doctors d ON d.doctor_id=s.doctor_id
                      WHERE s.slot_id=NEW.slot_id))
  WHERE appointment_id=NEW.appointment_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_appt_status_update
AFTER UPDATE OF status ON appointments
WHEN NEW.status='canceled'
BEGIN
  UPDATE appointment_slots SET status='available' WHERE slot_id=NEW.slot_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_appt_after_delete
AFTER DELETE ON appointments
BEGIN
  UPDATE appointment_slots SET status='available' WHERE slot_id=OLD.slot_id;
END;

/* ======================
   即时沟通（供 CommunicationWidget）
====================== */
CREATE TABLE IF NOT EXISTS conversations (
  conversation_id INTEGER PRIMARY KEY,
  patient_id INTEGER NOT NULL,
  doctor_id  INTEGER NOT NULL,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  UNIQUE (patient_id, doctor_id),
  FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
  FOREIGN KEY (doctor_id)  REFERENCES doctors(doctor_id)  ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS messages (
  message_id INTEGER PRIMARY KEY,
  conversation_id INTEGER NOT NULL,
  sender_type TEXT NOT NULL CHECK (sender_type IN ('patient','doctor','system')),
  sender_id INTEGER,                            -- 可选：冗余存发信人id
  content  TEXT NOT NULL,
  created_at TEXT NOT NULL DEFAULT (datetime('now')),
  FOREIGN KEY (conversation_id) REFERENCES conversations(conversation_id) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_messages_conv_time ON messages(conversation_id, created_at);

/* ======================
   健康评估（供 HealthAssessmentWidget）
====================== */
CREATE TABLE IF NOT EXISTS health_questions (
  question_id INTEGER PRIMARY KEY,
  text TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS health_options (
  option_id INTEGER PRIMARY KEY,
  question_id INTEGER NOT NULL,
  option_text TEXT NOT NULL,
  score INTEGER NOT NULL,
  FOREIGN KEY (question_id) REFERENCES health_questions(question_id) ON DELETE CASCADE
);
CREATE INDEX IF NOT EXISTS idx_health_options_question ON health_options(question_id);

CREATE TABLE IF NOT EXISTS health_results (
  result_id INTEGER PRIMARY KEY,
  min_score INTEGER NOT NULL,
  max_score INTEGER NOT NULL,
  summary TEXT NOT NULL,
  description TEXT,
  suggestions TEXT,
  background_color TEXT,
  text_color TEXT,
  CHECK (max_score >= min_score)
);

CREATE TABLE IF NOT EXISTS health_assessments (
  session_id INTEGER PRIMARY KEY,
  patient_id INTEGER NOT NULL,
  started_at TEXT NOT NULL DEFAULT (datetime('now')),
  total_score INTEGER,
  result_id INTEGER,
  FOREIGN KEY (patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
  FOREIGN KEY (result_id) REFERENCES health_results(result_id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS health_answers (
  session_id INTEGER NOT NULL,
  question_id INTEGER NOT NULL,
  option_id INTEGER NOT NULL,
  score INTEGER NOT NULL,
  PRIMARY KEY (session_id, question_id),
  FOREIGN KEY (session_id) REFERENCES health_assessments(session_id) ON DELETE CASCADE,
  FOREIGN KEY (question_id) REFERENCES health_questions(question_id) ON DELETE CASCADE,
  FOREIGN KEY (option_id) REFERENCES health_options(option_id) ON DELETE RESTRICT
);

/* 自动累计得分与匹配结果区间 */
CREATE TRIGGER IF NOT EXISTS trg_hans_after_insert
AFTER INSERT ON health_answers
BEGIN
  UPDATE health_assessments
  SET total_score = (SELECT COALESCE(SUM(score),0) FROM health_answers WHERE session_id=NEW.session_id)
  WHERE session_id=NEW.session_id;

  UPDATE health_assessments
  SET result_id = (SELECT result_id FROM health_results
                   WHERE total_score BETWEEN min_score AND max_score
                   LIMIT 1)
  WHERE session_id=NEW.session_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_hans_after_update
AFTER UPDATE OF score ON health_answers
BEGIN
  UPDATE health_assessments
  SET total_score = (SELECT COALESCE(SUM(score),0) FROM health_answers WHERE session_id=NEW.session_id)
  WHERE session_id=NEW.session_id;

  UPDATE health_assessments
  SET result_id = (SELECT result_id FROM health_results
                   WHERE total_score BETWEEN min_score AND max_score
                   LIMIT 1)
  WHERE session_id=NEW.session_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_hans_after_delete
AFTER DELETE ON health_answers
BEGIN
  UPDATE health_assessments
  SET total_score = (SELECT COALESCE(SUM(score),0) FROM health_answers WHERE session_id=OLD.session_id)
  WHERE session_id=OLD.session_id;

  UPDATE health_assessments
  SET result_id = (SELECT result_id FROM health_results
                   WHERE total_score BETWEEN min_score AND max_score
                   LIMIT 1)
  WHERE session_id=OLD.session_id;
END;

/* ======================
   药品检索（供 MedicineSearchWidget）
====================== */
CREATE TABLE IF NOT EXISTS medicines (
  medicine_id INTEGER PRIMARY KEY,
  name TEXT NOT NULL UNIQUE,
  description TEXT,
  type TEXT NOT NULL CHECK (type IN ('处方药','非处方药')),
  is_prescription INTEGER NOT NULL CHECK (is_prescription IN (0,1)),
  price REAL NOT NULL CHECK (price >= 0.0),
  specifications TEXT,
  manufacturer TEXT,
  effects TEXT,
  dosage TEXT,
  icon_color TEXT
);
CREATE INDEX IF NOT EXISTS idx_meds_name ON medicines(name);
CREATE INDEX IF NOT EXISTS idx_meds_type ON medicines(type, is_prescription);

/* ======================
   便捷视图（供列表/搜索使用）
====================== */
-- 可用号源（含最终费用）
CREATE VIEW IF NOT EXISTS v_available_slots AS
SELECT s.slot_id, s.doctor_id,
       d.name AS doctor_name, d.title, dep.name AS department,
       s.slot_start, s.slot_end,
       COALESCE(s.fee, d.consultation_fee) AS fee
FROM appointment_slots s
JOIN doctors d ON d.doctor_id = s.doctor_id
JOIN departments dep ON dep.department_id = d.department_id
WHERE s.status='available';

-- 医患会话列表（含最后一条消息与时间）
CREATE VIEW IF NOT EXISTS v_doctor_contact_list AS
SELECT
  c.conversation_id, c.patient_id,
  d.doctor_id, d.name AS doctor_name, dep.name AS department, d.title, d.is_online,
  (SELECT m.content FROM messages m WHERE m.conversation_id=c.conversation_id ORDER BY m.created_at DESC LIMIT 1) AS last_message,
  (SELECT m.created_at FROM messages m WHERE m.conversation_id=c.conversation_id ORDER BY m.created_at DESC LIMIT 1) AS last_time
FROM conversations c
JOIN doctors d ON d.doctor_id=c.doctor_id
JOIN departments dep ON dep.department_id=d.department_id;

-- 带年龄的患者档案视图（年龄按 365.2425 天/年估算）
CREATE VIEW IF NOT EXISTS v_patient_profile AS
SELECT
  patient_id, username, name, gender, birth_date,
  CASE WHEN birth_date IS NULL THEN NULL
       ELSE CAST((julianday('now') - julianday(birth_date)) / 365.2425 AS INTEGER)
  END AS age,
  id_number, blood_type, mobile, email, emergency_contact, emergency_phone,
  insurance_type, insurance_card, allergies, history, created_at
FROM patients;

/* ======================
   示例数据（可自行修改/扩展）
====================== */
INSERT OR IGNORE INTO departments(name) VALUES
 ('内科'),('外科'),('儿科'),('皮肤科'),('心内科');

INSERT OR IGNORE INTO patients(patient_id, username, name, gender, birth_date, mobile, email)
VALUES (1, 'Jane', 'Jane', '女', '2000-01-01', '13800000000', 'jane@example.com');

INSERT OR IGNORE INTO doctors(name, title, department_id, specialty, experience, consultation_fee, is_online)
SELECT '王强','主任医师', d.department_id,'冠心病、心律失常','从业20年，三甲医院',120,1 FROM departments d WHERE d.name='心内科'
UNION ALL
SELECT '李华','副主任医师', d.department_id,'皮炎湿疹、荨麻疹','从业15年',80,1 FROM departments d WHERE d.name='皮肤科'
UNION ALL
SELECT '赵敏','主治医师', d.department_id,'呼吸道/消化内科','从业8年',60,0 FROM departments d WHERE d.name='内科'
UNION ALL
SELECT '陈刚','主治医师', d.department_id,'小儿感冒、发热','从业10年',70,1 FROM departments d WHERE d.name='儿科';

-- 生成若干未来号源（注意当前日期：2025-08-31）
-- 2025-09-01 与 2025-09-02 不同医生不同时段
INSERT OR IGNORE INTO appointment_slots(doctor_id, slot_start, slot_end, fee, status)
SELECT doctor_id, '2025-09-01 09:00:00','2025-09-01 09:15:00', NULL, 'available' FROM doctors WHERE name='王强'
UNION ALL SELECT doctor_id, '2025-09-01 09:20:00','2025-09-01 09:35:00', NULL, 'available' FROM doctors WHERE name='王强'
UNION ALL SELECT doctor_id, '2025-09-01 10:00:00','2025-09-01 10:15:00', 100,  'available' FROM doctors WHERE name='李华'
UNION ALL SELECT doctor_id, '2025-09-02 14:00:00','2025-09-02 14:15:00', NULL, 'available' FROM doctors WHERE name='赵敏'
UNION ALL SELECT doctor_id, '2025-09-02 15:00:00','2025-09-02 15:15:00', NULL, 'available' FROM doctors WHERE name='陈刚';

-- 药品样例
INSERT OR IGNORE INTO medicines(name, description, type, is_prescription, price, specifications, manufacturer, effects, dosage, icon_color)
VALUES
 ('布洛芬缓释胶囊','解热镇痛','非处方药',0,18.80,'0.3g*12粒','XX制药','缓解轻中度疼痛/发热','成人一次1粒，一日2次','green'),
 ('阿莫西林胶囊','青霉素类抗生素','处方药',1,12.50,'0.5g*24粒','YY制药','敏感菌感染','遵医嘱','red'),
 ('氯雷他定片','抗过敏','非处方药',0,16.00,'10mg*10片','ZZ制药','缓解过敏性鼻炎、荨麻疹','成人一次1片，一日1次','blue'),
 ('奥美拉唑肠溶片','抑酸','处方药',1,22.00,'20mg*14片','AA制药','胃溃疡/反流','遵医嘱','orange');

-- Jane 与王强 建立会话与两条消息
INSERT OR IGNORE INTO conversations(conversation_id, patient_id, doctor_id, created_at)
SELECT 1, 1, d.doctor_id, datetime('now') FROM doctors d WHERE d.name='王强';

INSERT OR IGNORE INTO messages(conversation_id, sender_type, sender_id, content, created_at)
SELECT 1,'patient',1,'您好医生，我最近胸闷胸痛。', datetime('now','-10 minutes')
UNION ALL
SELECT 1,'doctor', (SELECT doctor_id FROM doctors WHERE name='王强'), '您好，请描述发作时间与伴随症状。', datetime('now','-9 minutes');

-- 健康评估：简单5题，4档评分(0/1/2/3)
INSERT OR IGNORE INTO health_questions(question_id, text) VALUES
 (1,'最近两周，入睡是否困难？'),
 (2,'白天是否感到乏力？'),
 (3,'食欲是否下降？'),
 (4,'是否经常情绪低落？'),
 (5,'是否经常感到焦虑紧张？');

INSERT OR IGNORE INTO health_options(question_id, option_text, score) VALUES
 (1,'从不',0),(1,'偶尔',1),(1,'经常',2),(1,'总是',3),
 (2,'从不',0),(2,'偶尔',1),(2,'经常',2),(2,'总是',3),
 (3,'从不',0),(3,'偶尔',1),(3,'经常',2),(3,'总是',3),
 (4,'从不',0),(4,'偶尔',1),(4,'经常',2),(4,'总是',3),
 (5,'从不',0),(5,'偶尔',1),(5,'经常',2),(5,'总是',3);

INSERT OR IGNORE INTO health_results(result_id, min_score, max_score, summary, description, suggestions, background_color, text_color) VALUES
 (1,0,5,'良好','整体状态良好','- 保持规律作息\n- 适量运动','rgba(76,175,80,0.1)','#2e7d32'),
 (2,6,9,'中等','有轻度不适','- 注意休息\n- 合理饮食','rgba(255,193,7,0.1)','##795548'),
 (3,10,15,'需关注','建议自我调整并观察','- 保持社交\n- 放松训练','rgba(255,87,34,0.1)','##bf360c');

-- 示例订单（用于 OnlinePaymentWidget 展示）：
-- 费用明细：挂号费25 + 诊疗费60 + 药费100 + 检查费120 = 305；折扣20 → 合计 285
INSERT OR IGNORE INTO orders(order_id, patient_id, department_id, doctor_id, discount, status, created_at)
SELECT 1, 1, dep.department_id, doc.doctor_id, 20.0, 'created', datetime('now')
FROM doctors doc JOIN departments dep ON dep.department_id = doc.department_id
WHERE doc.name='王强';

INSERT OR IGNORE INTO order_items(order_id, item_name, amount) VALUES
 (1,'挂号费',25.0),
 (1,'诊疗费',60.0),
 (1,'药费',100.0),
 (1,'检查费',120.0);
/* 触发器会把 orders.total_amount 自动更新为 285.0（经手工核算也为 285.0） */

-- 可选：插入一笔支付成功（微信 285 元），触发器会自动把订单置为 paid
INSERT OR IGNORE INTO payments(order_id, method, amount, status, created_at)
VALUES (1,'wechat',285.0,'success',datetime('now','-1 minutes'));

-- 示例预约：Jane 预约 王强 2025-09-01 09:00 档（将自动把该 slot 置为 booked，并回填 fee）
INSERT OR IGNORE INTO appointments(patient_id, doctor_id, slot_id, disease_description, order_id, created_at)
SELECT 1, s.doctor_id, s.slot_id, '胸闷胸痛一周，活动后加重', 1, datetime('now')
FROM appointment_slots s
JOIN doctors d ON d.doctor_id=s.doctor_id
WHERE d.name='王强' AND s.slot_start='2025-09-01 09:00:00' AND s.status='available';

COMMIT;
