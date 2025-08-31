# MedicalSystem

## Linux 环境依赖安装

如果你在 Linux 下运行本项目，且需要 SVG 支持（如药品界面、患者主窗口等），请先安装 Qt SVG 依赖：

```bash
sudo apt install libqt5svg5-dev
```

否则部分界面可能无法正常显示 SVG 图标。

| 功能 | 请求 type | 响应 type | 备注 |
|------|-----------|-----------|------|
| 登录 | login | login_result | {role, patient_id/doctor_id, name, ...} |
| 科室列表 | list_departments | list_departments_result | items:[{department_id,name}] |
| 医生列表 | list_doctors | list_doctors_result | 支持 dept/keyword/online/分页/排序 |
| 可约号源 | list_available_slots | list_available_slots_result | 可选 doctor_id、时间范围 |
| 预约下单 | book_appointment | book_appointment_result | 触发器会占位+回填费用 |
| 患者档案 | get_patient_profile | patient_profile_result | 来自 v_patient_profile 或 patients |
| 更新档案 | update_patient_profile | update_patient_profile_result | 成功/失败 |
| 医患联系人 | get_doctor_contacts | doctor_contacts_result | 来自视图 v_doctor_contact_list |
| 聊天记录 | get_chat_history | chat_history_result | 一次拉取该会话消息 |
| 发送消息 | send_message | send_message_result | 成功后前端追加一条 |
| 健康题目 | get_health_questions | health_questions_result | questions+options |
| 交卷评估 | submit_health_assessment | health_assessment_result | 返回匹配的 result 区间 |
| 药品搜索 | search_medicines | search_medicines_result | name/类型过滤 |
| 订单详情 | get_order_detail | order_detail_result | 明细+总额+状态 |
| 创建支付 | create_payment | payment_result | 成功会推动 orders.status=paid |
| 患者预约 | list_patient_appointments | patient_appointments_result | 列出我的预约 |
| 取消预约 | cancel_appointment | cancel_appointment_result | 触发器释放号源 |