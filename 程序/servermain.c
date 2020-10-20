#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>
#include<arpa/inet.h>
MYSQL mysql;
int online(char* userName) {//查询该用户是否在线
	printf("online?: %s\n",userName);
	char sqlStr[1024] = { 0 };
	MYSQL_RES* result;
	MYSQL_ROW row;
	sprintf(sqlStr, "select user_confd from userfd_tb where user_name = '%s';", userName);

	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//————————
	if (row = mysql_fetch_row(result)) {//如果查询有结果，返回其confd
		printf("%s online\n",userName);
		return atoi(row[0]);
	}
	else {//无结果则返回-1
		printf("%s no online\n",userName);
		return -1;
	}
}
int openDB(){//打开数据库连接
	mysql_init(&mysql);//初始化
	if (mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "Ochat_db", 0, NULL, 0) == NULL) {//如果mysql_real_connect()返回值为NULL，报错
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
}
int change_password(int fd, char* buf) {//修改密码
	char userName[48] = { 0 };
	char old_pw[16] = { 0 };
	char new_pw[16] = { 0 };
	char sqlStr[1024] = { 0 };
	sscanf(buf, "CPW|%[^|]|%[^|]|%s", userName, old_pw, new_pw);
	sprintf(sqlStr, "select user_password from user_tb where user_name = '%s'", userName);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	if (row = mysql_fetch_row(result)) {//用户名存在
		if (strcmp(row[0], old_pw) == 0) {//密码正确
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
	else {//用户名不存在
		send(fd, "3|ERR|user not exist", 18, 0);
		return -1;
	}
}
int del_fri(int fd, char* buf) {//删除好友
	char friName[32] = { 0 };
	char target_user[48] = { 0 };
	char sender[48] = { 0 };
	char sqlStr[1024] = { 0 };
	char sendmes[1024] = "DDD|";
	sscanf(buf, "DLF|%[^|]|%s", target_user, sender);
	sprintf(sqlStr, "select * from friend_tb where usera_name = '%s' and userb_name = '%s';", sender, target_user);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	if (mysql_fetch_row(result)) {//两人确实为好友
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
	//向该用户发送其好友列表
	memset(sqlStr, 0, 1024);
	sprintf(sqlStr, "select userb_name from friend_tb where usera_name = '%s'", sender);
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	int num_rows = mysql_num_rows(result);
	if (num_rows == 0) {//当前用户没有好友
		send(fd, "FRI|", 4, 0);
	}
	else {//当前用户有好友
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
int friend_request(int fd, char* buf) {//被申请用户处理申请后的反馈
	char attitude;
	char req_sender[32] = { 0 };
	char req_target[32] = { 0 };
	char send_mes[1024] = { 0 };
	char sqlStr[1024] = { 0 };
	int getfd = 0;
	int len = 0;
	sscanf(buf, "ATF|%c|%[^|]|%s", &attitude, req_sender, req_target);
	getfd = online(req_sender);
	if (getfd != -1) {//申请者在线
		if (attitude == '0') {//被申请者态度为拒绝
			sprintf(send_mes, "REF|%s", req_target);
			len = strnlen(send_mes);
			send(getfd, send_mes, len, 0);
		}
		else if(attitude == '1'){//被申请者态度为接受
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
	else {//申请者不在线，先将申请反馈存入数据库accreq_tb
		sprintf(sqlStr, "insert into accreq_tb values('%s','%s','%c');", req_target, req_sender, attitude);
		if (mysql_query(&mysql, sqlStr) != 0) {
			printf("%s\n", mysql_error(&mysql));
			return -1;
		}
	}
	
}
int add_friend(int fd,char *buf) {//添加好友
	char target_user[32] = { 0 };
	char sender[32] = { 0 };
	char sqlStr[1024] = { 0 };
	char send_mes[1024] = { 0 };
	sscanf(buf, "FAF|%[^|]|%s", target_user, sender);
	sprintf(sqlStr, "select user_name from user_tb where user_name = '%s'", target_user);
	MYSQL_RES* result;
	MYSQL_ROW row;
	//mysql查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	if (mysql_fetch_row(result)) {//该用户存在
		int getfd = online(target_user);
		if (getfd != -1) {//该用户在线
			sprintf(send_mes, "FRR|%s", sender);
			int len = strlen(send_mes);
			send(getfd, send_mes, len, 0);
		}
		else {//该用户不在线
			sprintf(sqlStr, "insert into firreq_tb values('%s','%s')", sender,target_user);
			if (mysql_query(&mysql, sqlStr) != 0) {
				printf("%s\n", mysql_error(&mysql));
				return -1;
			}
		}
	}
	else {//该用户不存在
		send(fd, "EAD|user not exist", 18, 0);
		printf("error when add friend ");
		return -1;
	}
}
int send_mes(int fd, char* buf) {//收到客户端传来的“CWH|目标用户|消息内容|发送用户”消息时的处理函数
	//用于将消息发给已登录的用户或将将消息暂存于数据库当中
	char target_user[48] = { 0 };//储存目标用户用户名的数组
	char message[1024] = { 0 };//储存用户发送的消息的数组
	char send_user[48] = { 0 };//储存发送者用户名的数组
	char sqlStr[1024] = { 0 };//表示SQL语句的字符串
	char send_mes[1024] = { 0 };
	int getfd = 0;//当前已登录的目标用户的句柄
	sscanf(buf, "CWH|%[^|]|%[^|]|%s", target_user, message, send_user);
	getfd = online(target_user);
	printf("send_mes:fd:%d\n",getfd);
	if (getfd != -1) {//接收者在线，直接发送
		sprintf(send_mes, "REC|%s|%s", message, send_user);
		printf("%s\n",send_mes);
		int len = strlen(send_mes);
		printf("send_mes_len:%d\n",len);
		int has = send(getfd, send_mes, len, 0);
		printf("send return:%d\n",has);
	}
	else {//接收者不在线，先将消息存入数据库
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
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//————————
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
int client_close(int fd) {//用于当有客户端断开连接时的处理函数
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
int reg(int fd,char *buf){//用于处理注册
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
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if(result == NULL){
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//————————
	if(mysql_fetch_row(result)){//该用户名已被注册
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
int login_(int fd,char *buf){//处理登录，并在登录后向该用户发送其好友列表以及未读消息
	
	char userName[32] = { 0 };
	char friName[32] = { 0 };
	char passWord[32] = { 0 };
	sscanf(buf, "LOG|%[^|]|%s", userName, passWord);
	char sqlStr[1024] = { 0 };
	char sendmes[1024] = "FRI";
	sprintf(sqlStr, "select * from user_tb where user_name = '%s'", userName);
	MYSQL_RES *result;
	MYSQL_ROW row;
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if(result == NULL){
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//————————
	if (row = mysql_fetch_row(result)) {//用户名存在
		if(strcmp(row[1],passWord) == 0){//密码正确
			send(fd, "2|SUC|login successfully ", 25, 0);
		}
		else{
			send(fd, "2|ERR|wrong password", 20, 0);
			return -1;
		}
	}
	else{//用户名不存在
		send(fd, "2|ERR|user not exist", 18, 0);
		return -1;
	}
	printf("user %s:%d has logined\n", userName, fd);
	//至此用户登录成功
	//将该已登录的用户的句柄储存进数据库
	sprintf(sqlStr, "insert into userfd_tb values('%s',%d) ", userName, fd);
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//向该用户发送其好友列表
	memset(sqlStr, 0, 1024);
	sprintf(sqlStr, "select userb_name from friend_tb where usera_name = '%s'", userName);
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	int num_rows = mysql_num_rows(result);
	if (num_rows == 0) {//当前用户没有好友
		send(fd, "FRI|", 4, 0);
	}
	else {//当前用户有好友
		while (row = mysql_fetch_row(result))
		{
			sprintf(friName, "|%s", row[0]);
			strcat(sendmes, friName);
		}
		int len = strlen(sendmes) + 3;
		send(fd, sendmes, len, 0);
	}
	//开始向该用户发送未处理好友申请
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
	//——————————
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
	
	//开始向该用户发送未读的好友申请结果
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
	//——————————
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
	
	//开始向该用户发送未读消息
	sprintf(sqlStr, "select message,sender_name from message_tb where target_name = '%s';", userName);
	memset(sendmes, 0, 1024);
	//MySQL查询错误处理
	if (mysql_query(&mysql, sqlStr) != 0) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	result = mysql_store_result(&mysql);
	if (result == NULL) {
		printf("%s\n", mysql_error(&mysql));
		return -1;
	}
	//——————————
	num_rows = mysql_num_rows(result);
	if (num_rows > 0) {//当前用户有未读消息
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
	openDB();//连接数据库
	int sockfd = socket(AF_INET,SOCK_STREAM,0);//获取服务器自身的套接字
	struct sockaddr_in myaddr;
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8899);
	myaddr.sin_addr.s_addr = inet_addr("192.168.43.216");
	if (bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1) {//将套接字与IP等绑定
		perror("bind");
		return -1;		
	}
	listen(sockfd,10);//套接字监听
	fd_set allset,rset;//定义两个集合
	int maxfd = 0;
	FD_ZERO(&allset);
	FD_SET(sockfd,&allset);//将服务器套接字放入allset
	if(sockfd > maxfd) maxfd = sockfd;
	while(1){
		rset = allset;
		int nready = select(maxfd + 1,&rset,NULL,NULL,NULL);//执行select函数，获得改变后的rset
		if(nready == -1){
			perror("select");
			return -1;
		}
		if(FD_ISSET(sockfd,&rset)){//判断服务器套接字是否已被放入rset集合中
			int confd = accept(sockfd,NULL,NULL);//是，则开始连接第一个客户端，获得通道的句柄
			FD_SET(confd,&allset);//将句柄放入allset中
			if(confd > maxfd) maxfd = confd;
			clifd[curi++] = confd;//将confd储存起来
		}
		for(i = 0;i < curi;i++){
			if(FD_ISSET(clifd[i],&rset)){//判断已经储存起来的句柄是否已被放入rset集合中
				memset(buf,0,sizeof(buf));
				int recv_len = recv(clifd[i],buf,sizeof(buf),0);//接收消息
				if (recv_len == 0) {//该客户端已关闭
					FD_CLR(clifd[i], &allset);//客户端若关闭需要把conn从allset中删除
					client_close(clifd[i]);
				}
				printf("confd = %d:%s\n",clifd[i],buf);
				char fragstr[3] = {0};
				sscanf(buf,"%3s",fragstr);
				if(strcmp(fragstr,"REG") == 0){//注册
					reg(clifd[i],buf);
				}
				else if(strcmp(fragstr,"LOG") == 0){//登录
					login_(clifd[i],buf);				
				}
				else if (strcmp(fragstr, "CWH") == 0) {//用户向另一用户发送信息
					send_mes(clifd[i],buf);
				}
				else if (strcmp(fragstr, "FAF") == 0) {//添加好友
					add_friend(clifd[i], buf);
				}
				else if (strcmp(fragstr, "ATF") == 0) {//好友申请通过情况
					friend_request(clifd[i], buf);
				}
				else if (strcmp(fragstr, "DLF") == 0) {//删除好友
					del_fri(clifd[i], buf);
				}
				else if (strcmp(fragstr, "CPW") == 0) {//修改密码
					change_password(clifd[i], buf);
				}
				else if (strcmp(fragstr, "GFI") == 0) {//获取好友信息
					send_fri_infor(clifd[i], buf);
				}
			}
		}	
	}	
}
