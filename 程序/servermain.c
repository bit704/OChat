#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>
#include<arpa/inet.h>
MYSQL mysql;
int online(char* userName) {//��ѯ���û��Ƿ�����
	printf("online?: %s\n",userName);
	char sqlStr[1024] = { 0 };
	MYSQL_RES* result;
	MYSQL_ROW row;
	sprintf(sqlStr, "select user_confd from userfd_tb where user_name = '%s';", userName);

	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//����������������
	if (row = mysql_fetch_row(result)) {//�����ѯ�н����������confd
		printf("%s online\n",userName);
		return atoi(row[0]);
	}
	else {//�޽���򷵻�-1
		printf("%s no online\n",userName);
		return -1;
	}
}
int openDB(){//�����ݿ�����
	mysql_init(&mysql);//��ʼ��
	if (mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "Ochat_db", 0, NULL, 0) == NULL) {//���mysql_real_connect()����ֵΪNULL������
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
}
int change_password(int fd, char* buf) {//�޸�����
	char userName[48] = { 0 };
	char old_pw[16] = { 0 };
	char new_pw[16] = { 0 };
	char sqlStr[1024] = { 0 };
	sscanf(buf, "CPW|%[^|]|%[^|]|%s", userName, old_pw, new_pw);
	sprintf(sqlStr, "select user_password from user_tb where user_name = '%s'", userName);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	if (row = mysql_fetch_row(result)) {//�û�������
		if (strcmp(row[0], old_pw) == 0) {//������ȷ
			sprintf(sqlStr, "update user_tb set user_password = '%s' where user_name = '%s'", new_pw, userName);
			if (mysql_query(&mysql, sqlStr) != 0) {
				printf("%s\n", mysql_error(&mysql));
				return -1;
			}
			send(fd, "3|SUC|", 6, 0);
		}
		else {
			send(fd, "3|ERR|wrong password", 20, 0);
			return -1;
		}
	}
	else {//�û���������
		send(fd, "3|ERR|user not exist", 18, 0);
		return -1;
	}
}
int del_fri(int fd, char* buf) {//ɾ������
	char friName[32] = { 0 };
	char target_user[48] = { 0 };
	char sender[48] = { 0 };
	char sqlStr[1024] = { 0 };
	char sendmes[1024] = "DDD|";
	sscanf(buf, "DLF|%[^|]|%s", target_user, sender);
	sprintf(sqlStr, "select * from friend_tb where usera_name = '%s' and userb_name = '%s';", sender, target_user);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	if (mysql_fetch_row(result)) {//����ȷʵΪ����
		sprintf(sqlStr, "delete from friend_tb where usera_name = '%s' and userb_name = '%s';", sender, target_user);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
		sprintf(sqlStr, "delete from friend_tb where usera_name = '%s' and userb_name = '%s';", target_user, sender);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
		printf("delete friend between %s and  %s\n",sender,target_user);
	}
	//����û�����������б�
	memset(sqlStr, 0, 1024);
	sprintf(sqlStr, "select userb_name from friend_tb where usera_name = '%s'", sender);
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	int num_rows = mysql_num_rows(result);
	if (num_rows == 0) {//��ǰ�û�û�к���
		send(fd, "FRI|", 4, 0);
	}
	else {//��ǰ�û��к���
		sprintf(sendmes,"DDD|%s",target_user);
		while (row = mysql_fetch_row(result))
		{
			sprintf(friName, "|%s", row[0]);
			strcat(sendmes, friName);
		}
		int len = strlen(sendmes);
		send(fd, sendmes, len, 0);
		printf("%s\n",sendmes);
	}
}
int friend_request(int fd, char* buf) {//�������û����������ķ���
	char attitude;
	char req_sender[32] = { 0 };
	char req_target[32] = { 0 };
	char send_mes[1024] = { 0 };
	char sqlStr[1024] = { 0 };
	int getfd = 0;
	int len = 0;
	sscanf(buf, "ATF|%c|%[^|]|%s", &attitude, req_sender, req_target);
	getfd = online(req_sender);
	if (getfd != -1) {//����������
		if (attitude == '0') {//��������̬��Ϊ�ܾ�
			sprintf(send_mes, "REF|%s", req_target);
			len = strnlen(send_mes);
			send(getfd, send_mes, len, 0);
		}
		else if(attitude == '1'){//��������̬��Ϊ����
			sprintf(send_mes, "ADF|%s", req_target);
			len = strnlen(send_mes);
			send(getfd, send_mes, len, 0);
		}
		sprintf(sqlStr, "delete from firreq_tb where revc = '%s' and sender = '%s';", req_target, req_sender);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
		sprintf(sqlStr, "insert into friend_tb values('%s','%s');", req_sender, req_target);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
		sprintf(sqlStr, "insert into friend_tb values('%s','%s');", req_target, req_sender);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	else {//�����߲����ߣ��Ƚ����뷴���������ݿ�accreq_tb
		sprintf(sqlStr, "insert into accreq_tb values('%s','%s','%c');", req_target, req_sender, attitude);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	
}
int add_friend(int fd,char *buf) {//��Ӻ���
	char target_user[32] = { 0 };
	char sender[32] = { 0 };
	char sqlStr[1024] = { 0 };
	char send_mes[1024] = { 0 };
	sscanf(buf, "FAF|%[^|]|%s", target_user, sender);
	sprintf(sqlStr, "select user_name from user_tb where user_name = '%s'", target_user);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	if (mysql_fetch_row(result)) {//���û�����
		int getfd = online(target_user);
		if (getfd != -1) {//���û�����
			sprintf(send_mes, "FRR|%s", sender);
			int len = strlen(send_mes);
			send(getfd, send_mes, len, 0);
		}
		else {//���û�������
			sprintf(sqlStr, "insert into firreq_tb values('%s','%s')", sender,target_user);
			if (mysql_query(&mysql, sqlStr) != 0) {
				printf("%s\n", mysql_error(&mysql));
				return -1;
			}
		}
	}
	else {//���û�������
		send(fd, "EAD|user not exist", 18, 0);
		printf("error when add friend ");
		return -1;
	}
}
int send_mes(int fd, char* buf) {//�յ��ͻ��˴����ġ�CWH|Ŀ���û�|��Ϣ����|�����û�����Ϣʱ�Ĵ�����
	//���ڽ���Ϣ�����ѵ�¼���û��򽫽���Ϣ�ݴ������ݿ⵱��
	char target_user[48] = { 0 };//����Ŀ���û��û���������
	char message[1024] = { 0 };//�����û����͵���Ϣ������
	char send_user[48] = { 0 };//���淢�����û���������
	char sqlStr[1024] = { 0 };//��ʾSQL�����ַ���
	char send_mes[1024] = { 0 };
	int getfd = 0;//��ǰ�ѵ�¼��Ŀ���û��ľ��
	sscanf(buf, "CWH|%[^|]|%[^|]|%s", target_user, message, send_user);
	getfd = online(target_user);
	printf("send_mes:fd:%d\n",getfd);
	if (getfd != -1) {//���������ߣ�ֱ�ӷ���
		sprintf(send_mes, "REC|%s|%s", message, send_user);
		printf("%s\n",send_mes);
		int len = strlen(send_mes);
		printf("send_mes_len:%d\n",len);
		int has = send(getfd, send_mes, len, 0);
		printf("send return:%d\n",has);
	}
	else {//�����߲����ߣ��Ƚ���Ϣ�������ݿ�
		sprintf(sqlStr, "insert into message_tb values('%s','%s','%s')", target_user, message, send_user);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	

}
int send_fri_infor(int fd, char* buf) {
	char fri_name[48] = { 0 };
	char sqlStr[1024] = { 0 };
	char sendmes[1024] = { 0 };
	sscanf(buf, "GFI|%s", fri_name);
	sprintf(sqlStr, "select sex,address,dob from user_tb where user_name = '%s';", fri_name);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//����������������
	if (row = mysql_fetch_row(result)) {
		sprintf(sendmes, "SFI|%s|%s|%s|%s", fri_name,row[0], row[1], row[2]);
		int len = strlen(sendmes);
		int l = send(fd, sendmes, len, 0);
		if(l == len){
			printf("send (%s) suc!\n",sendmes);		
		}
	}
	else {
		printf("this friend %s not exist\n", fri_name);
	}
}
int client_close(int fd) {//���ڵ��пͻ��˶Ͽ�����ʱ�Ĵ�����
	char userName[48] = { 0 };
	char sqlStr[1024] = { 0 };
	sprintf(sqlStr, "delete from userfd_tb where user_confd = %d", fd);
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		printf("an error happened when a client close!\n");
		return -1;
	}
	printf("confd = %d leave\n",fd);
}
int reg(int fd,char *buf){//���ڴ���ע��
	char userName[32] = { 0 };
	char passWord[32] = { 0 };
	char sex[5] = { 0 };
	char address[48] = { 0 };
	char dob[11] = { 0 };
	sscanf(buf, "REG|%[^|]|%[^|]|%[^|]|%[^|]|%s", userName, passWord, sex, address, dob);
	char sqlStr[1024] = { 0 };
	sprintf(sqlStr, "select * from user_tb where user_name = '%s'", userName);

	MYSQL_RES *result;
	MYSQL_ROW row;
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if(result == NULL){
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//����������������
	if(mysql_fetch_row(result)){//���û����ѱ�ע��
		send(fd, "1|ERR|user exist", 16, 0);
		return -1;
	}
	memset(sqlStr, 0, 1024);
	sprintf(sqlStr, "insert into user_tb values('%s','%s','%s','%s','%s')", userName, passWord, sex, address, dob);
	if(mysql_query(&mysql,sqlStr) != 0){
		printf("%s\n",mysql_error(&mysql));
		send(fd,"1|ERR|system error",18,0);
		return -1;
	}
	send(fd,"1|SUC|successful",14,0);
	printf("%s reg successful\n", userName);
	return 0;
}
int login_(int fd,char *buf){//�����¼�����ڵ�¼������û�����������б��Լ�δ����Ϣ
	
	char userName[32] = { 0 };
	char friName[32] = { 0 };
	char passWord[32] = { 0 };
	sscanf(buf, "LOG|%[^|]|%s", userName, passWord);
	char sqlStr[1024] = { 0 };
	char sendmes[1024] = "FRI";
	sprintf(sqlStr, "select * from user_tb where user_name = '%s'", userName);
	MYSQL_RES *result;
	MYSQL_ROW row;
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if(result == NULL){
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//����������������
	if (row = mysql_fetch_row(result)) {//�û�������
		if(strcmp(row[1],passWord) == 0){//������ȷ
			send(fd, "2|SUC|login successfully ", 25, 0);
		}
		else{
			send(fd, "2|ERR|wrong password", 20, 0);
			return -1;
		}
	}
	else{//�û���������
		send(fd, "2|ERR|user not exist", 18, 0);
		return -1;
	}
	printf("user %s:%d has logined\n", userName, fd);
	//�����û���¼�ɹ�
	//�����ѵ�¼���û��ľ����������ݿ�
	sprintf(sqlStr, "insert into userfd_tb values('%s',%d) ", userName, fd);
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//����û�����������б�
	memset(sqlStr, 0, 1024);
	sprintf(sqlStr, "select userb_name from friend_tb where usera_name = '%s'", userName);
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	int num_rows = mysql_num_rows(result);
	if (num_rows == 0) {//��ǰ�û�û�к���
		send(fd, "FRI|", 4, 0);
	}
	else {//��ǰ�û��к���
		while (row = mysql_fetch_row(result))
		{
			sprintf(friName, "|%s", row[0]);
			strcat(sendmes, friName);
		}
		int len = strlen(sendmes) + 3;
		send(fd, sendmes, len, 0);
	}
	//��ʼ����û�����δ�����������
	sprintf(sqlStr, "select * from firreq_tb where revc = '%s';", userName);
	memset(sendmes, 0, 1024);
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	num_rows = mysql_num_rows(result);
	if (num_rows > 0) {
		while (row = mysql_fetch_row(result))
		{
			sprintf(sendmes, "FRR|%s", row[0]);
			int len = strlen(sendmes);
			send(fd, sendmes, len, 0);
		}
		sprintf(sqlStr, "delete from firreq_tb where revc = '%s';", userName);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	
	//��ʼ����û�����δ���ĺ���������
	sprintf(sqlStr, "select * from accreq_tb where req_target = '%s';", userName);
	memset(sendmes, 0, 1024);
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	num_rows = mysql_num_rows(result);
	if (num_rows > 0) {
		while (row = mysql_fetch_row(result))
		{
			if (row[2] == "0") {
				sprintf(sendmes, "REF|%s", row[0]);
				int len = strlen(sendmes);
				send(fd, sendmes, len, 0);
			}
			else if (row[2] == "0") {
				sprintf(sendmes, "ADF|%s", row[0]);
				int len = strlen(sendmes);
				send(fd, sendmes, len, 0);
			}
		}
		sprintf(sqlStr, "delete from accreq_tb where req_target = '%s';", userName);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	
	//��ʼ����û�����δ����Ϣ
	sprintf(sqlStr, "select message,sender_name from message_tb where target_name = '%s';", userName);
	memset(sendmes, 0, 1024);
	//MySQL��ѯ������
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//��������������������
	num_rows = mysql_num_rows(result);
	if (num_rows > 0) {//��ǰ�û���δ����Ϣ
		while (row = mysql_fetch_row(result))
		{
			sprintf(sendmes, "REC|%s|%s", row[1], row[0]);
			int len = strlen(sendmes);
			int has = send(fd, sendmes, len, 0);
		}
		sprintf(sqlStr, "delete from message_tb where target_name = '%s';", userName);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}

}
int main(){
	int clifd[1024] = { 0 };
	int curi = 0, i = 0;
	char buf[1024] = { 0 };
	openDB();//�������ݿ�
	int sockfd = socket(AF_INET,SOCK_STREAM,0);//��ȡ������������׽���
	struct sockaddr_in myaddr;
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8899);
	myaddr.sin_addr.s_addr = inet_addr("192.168.43.216");
	if (bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1) {//���׽�����IP�Ȱ�
		perror("bind");
		return -1;		
	}
	listen(sockfd,10);//�׽��ּ���
	fd_set allset,rset;//������������
	int maxfd = 0;
	FD_ZERO(&allset);
	FD_SET(sockfd,&allset);//���������׽��ַ���allset
	if(sockfd > maxfd) maxfd = sockfd;
	while(1){
		rset = allset;
		int nready = select(maxfd + 1,&rset,NULL,NULL,NULL);//ִ��select��������øı���rset
		if(nready == -1){
			perror("select");
			return -1;
		}
		if(FD_ISSET(sockfd,&rset)){//�жϷ������׽����Ƿ��ѱ�����rset������
			int confd = accept(sockfd,NULL,NULL);//�ǣ���ʼ���ӵ�һ���ͻ��ˣ����ͨ���ľ��
			FD_SET(confd,&allset);//���������allset��
			if(confd > maxfd) maxfd = confd;
			clifd[curi++] = confd;//��confd��������
		}
		for(i = 0;i < curi;i++){
			if(FD_ISSET(clifd[i],&rset)){//�ж��Ѿ����������ľ���Ƿ��ѱ�����rset������
				memset(buf,0,sizeof(buf));
				int recv_len = recv(clifd[i],buf,sizeof(buf),0);//������Ϣ
				if (recv_len == 0) {//�ÿͻ����ѹر�
					FD_CLR(clifd[i], &allset);//�ͻ������ر���Ҫ��conn��allset��ɾ��
					client_close(clifd[i]);
				}
				printf("confd = %d:%s\n",clifd[i],buf);
				char fragstr[3] = {0};
				sscanf(buf,"%3s",fragstr);
				if(strcmp(fragstr,"REG") == 0){//ע��
					reg(clifd[i],buf);
				}
				else if(strcmp(fragstr,"LOG") == 0){//��¼
					login_(clifd[i],buf);				
				}
				else if (strcmp(fragstr, "CWH") == 0) {//�û�����һ�û�������Ϣ
					send_mes(clifd[i],buf);
				}
				else if (strcmp(fragstr, "FAF") == 0) {//��Ӻ���
					add_friend(clifd[i], buf);
				}
				else if (strcmp(fragstr, "ATF") == 0) {//��������ͨ�����
					friend_request(clifd[i], buf);
				}
				else if (strcmp(fragstr, "DLF") == 0) {//ɾ������
					del_fri(clifd[i], buf);
				}
				else if (strcmp(fragstr, "CPW") == 0) {//�޸�����
					change_password(clifd[i], buf);
				}
				else if (strcmp(fragstr, "GFI") == 0) {//��ȡ������Ϣ
					send_fri_infor(clifd[i], buf);
				}
			}
		}	
	}	
}
