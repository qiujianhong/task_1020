1. ������־�����Ϊjournald
2. fix bug������װ�����ֺͰ�װ����/binĿ¼��ʵ��bin�ļ����ֲ�һ��ʱ��appm -I ʱ��app״̬��ʾ�쳣
3. ���ssal��ǩ�ӿڣ�container/appm��װ/����APPʱ�������APP����ǩ�����ӡ��ǩ�����������ǩʧ�ܺ󣬲����˳��������������С�
4. ȥ��APP���������mem/cpuʹ���ʴ�ӡ��ֻ��Խ�޺�Խ�޻ָ�ʱ��ӡ
5. container����buffer����ֹ��Ϊ���� -v/-dev ����ʱ��������ʧ
6. container��ǿ���й����еķ����Լ�⣬���쳣ʱ��������ӡʧ��ԭ��

20220827 add:
7. ����container -p�˿�ӳ���¼�������¹淶����ƥ��
8. ȡ��appSignTool_arm, ʹ���������ǵķ�ʽ��ֹ�Ƿ�APP�������˹�����Ҫ����container���������������Ѱ�װ������Ҳ��Ҫ���°�װ
9. ��������������ļ������������������ļ���СΪ0/�ļ���ʽ�𻵣�

20220828 add:
10. ResMonitor���������׼��ʱ���120s����Ϊ60s����ResMonitor�������ǰ60s��ִ��������Դ��أ�ResMonitor���������°�װһ����������Ӱ�װ��������ʼ��60s�ڲ���ش�������������60s����������������ء�
11. ��������������µ�20220828��SecRenif��ѹ����ǩAPP��װ���󣬰�װ�����·������Ϊ/backup/app_back/app_name/��ԭ��Ϊ/backup/app_back/
12. ����appm -i/container install with APP���ַ�ʽ��װAPPʱ��

20220829 add:
13. ��װ�����·������Ϊ/backup/app_back/app_name/bin
14. ����container��������APP��װʱ����װ������Ҫ��ʱ2s���ȴ�appm_docker����MQTT������������ʧ�ܣ�

20220830 add:
15. ssl��ǩ���������ϣ��Ѿ��ܴ�SecRenif�ɹ���ȡ��ǩ�����
16. �ս�License/Signature������жϣ���֤ʧ��������/��װAPPʧ��

20220905 add:
17. License��֤��HAL Licnese��ȡ�ӿڸ��£�int (*devinfo_read_applicense_index)(struct tag_DEVINFO_DEVICE *dev, uint32 index, char *appno, char *appname, char *license, int max_license_size);


