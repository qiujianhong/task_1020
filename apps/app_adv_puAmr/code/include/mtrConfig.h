//��������Ч��

#ifndef MTRCONFIG_H
#define MTRCONFIG_H

//ͨ��0��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG0 "MTRCONFIG0"
//ͨ��1��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG1 "MTRCONFIG1"
//ͨ��2��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG2 "MTRCONFIG2"
//ͨ��3��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG3 "MTRCONFIG3"
//ͨ��4��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG4 "MTRCONFIG4"
//ͨ��5��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG5 "MTRCONFIG5"
//ͨ��6��Ӧ�Ĳ�������Ϣ�ļ�
#define MTRCONFIG6 "MTRCONFIG6"

//ͨ��31��Ӧ�Ĳ�������Ϣ�ļ�  //�ز�������
#define MTRCONFIG31 "MTRCONFIG31"

//��������
#define MTRVALID_COUNT_LEN     2
//����ƫ����
#define MTRVALID_COUNT_OFFSET  0

//����ִ�е������
#define MTRPERFORM_INDEX_LEN 2
//����ִ�е�����ƫ����
#define MTRPERFORM_INDEX_OFFSET (MTRVALID_COUNT_LEN+MTRVALID_COUNT_OFFSET)

//�����־����        0x55��ʼ���� 0xAA�������� ������Ч
#define MTRPERFORM_TAG_LEN 1
//�����־ƫ����
#define MTRPERFORM_TAG_OFFSET (MTRPERFORM_INDEX_LEN+MTRPERFORM_INDEX_OFFSET)

//������ų���
#define MTRINDEX_NO_LEN 2
//�������ƫ����
#define MTRINDEX_NO_OFFSET(a) (MTRPERFORM_TAG_LEN+MTRPERFORM_TAG_OFFSET+(a)*2)

#endif


