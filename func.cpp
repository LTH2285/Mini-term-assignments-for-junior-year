#include "func.h"
#include "utls.h"
#include <QDebug>
#include <Init.h>

Func::Func()
{
}

//查询用户IP是否存在
bool Func::checkUserIDExist(int userID)
{
    Utls utls;
    QSqlQuery query = utls.researchi("user_information_table", "userID=" + QString::number(userID));

    if (query.first()) {
        qDebug() << "UserID exists:" << userID;
        return true;
    } else {
        qDebug() << "UserID does not exist:" << userID;
        return false;
    }
}

//注册新账户
int Func::account_register(const QString username, const QString password)
{
    Utls utls;
    int existingrows = utls.getRowNumbers("user_information_table");
    if (existingrows < 0) {
        qDebug() << "Error fetching existing row numbers.";
        return -1; // Error code
    }

    int id = existingrows + 1;
    QString values = QString("%1, '%2', '%3', NULL").arg(id).arg(username).arg(password);
    bool status = utls.creati("user_information_table", values);
    if (!status) {
        qDebug() << "Error registering the account.";
        return -1; // Error code
    } else {
        qDebug() << "Successfully registered the account with ID:" << id;
        return id; // Return the ID of the newly registered account
    }
}

//注册将用户名密码写入数据库
bool Func::loginFunction(int userID, const QString password)
{
    Utls utls;
    QSqlQuery query = utls.researchi("user_information_table", "userID=" + QString::number(userID) + " AND passWord='" + password + "'");
    qDebug() << "userID=" + QString::number(userID) + " AND passWord='" + password + "'";
    qDebug() << "Query error:" << query.lastError().text();
    if (query.first()) {
        qDebug() << "Login successful for userID:" << userID;
        return true;
    } else {
        qDebug() << "Login failed for userID:" << userID;
        return false;
    }
}

// 主界面查询当前用户个人信息(photo、userID、userName)
QSqlQuery Func::getUserInfo(int userID)
{
//    Init init;
    QSqlQuery query;

    QString sql = "SELECT photo, userName FROM user_information_table WHERE userID = :userID";
    query.prepare(sql);

    query.bindValue(":userID", userID);
    query.exec();
//    if (query.exec() && query.next())
//    {
//        QSqlQuery userInfoQuery;
//        userInfoQuery.bindValue(":photo", query.value(0));
//        userInfoQuery.bindValue(":userID", userID);
//        userInfoQuery.bindValue(":userName", query.value(1));

//       return userInfoQuery;
//    }
    return query;
}

//主界面查询当前在线好友
QSqlQuery Func::getOnlineFriends(int userID)
{
//    Init init;
//    QSqlDatabase db = init.createDatabase();
    QSqlQuery query;

    QString sql = "SELECT DISTINCT friendID FROM friend_relationship_table "
                  "WHERE userIP = :userID AND passed = 1 "
                  "AND friendID IN (SELECT userID FROM user_login_table)";
    query.prepare(sql);
    query.bindValue(":userID", userID);

    if (query.exec())
    {
//        db.close();
        return query;
    }

//    db.close();
    return QSqlQuery(); // Return an empty query if no online friends found;
}

//主界面查询当前在线好友数
int Func::getOnlineFriendsCount(int userID)
{
//    Init init;
//    QSqlDatabase db = init.createDatabase();
    QSqlQuery query;

    QString sql = "SELECT COUNT(DISTINCT friendID) FROM friend_relationship_table "
                  "WHERE userIP = :userID AND passed = 1 "
                  "AND friendID IN (SELECT userID FROM user_login_table)";
    query.prepare(sql);
    query.bindValue(":userID", userID);

    if (query.exec() && query.next())
    {
        int count = query.value(0).toInt();
//        db.close();
        return count;
    }

//    db.close();
    return 0; // Return 0 if no online friends found;
}

//查询未通过的好友申请
QSqlQuery Func::getPendingFriendRequests(int userID)
{
    Utls utls;
    QSqlQuery query = utls.researchi("friend_relationship_table", "userIP=" + QString::number(userID) + " AND passed=0");

    return query;
}

// 查询在群聊内的用户
QSqlQuery Func::getGroupChatMembers(int groupID)
{
    Utls utls;
    QSqlQuery query = utls.researchi("group_member_table", "groupID=" + QString::number(groupID));

    return query;
}

// 查询聊天历史记录（单聊）
QSqlQuery Func::getChatHistory(int userID1, int userID2)
{
    Utls utls;
    QString conditions = "sendID=" + QString::number(userID1) + " AND recvID=" + QString::number(userID2)
                         + " OR sendID=" + QString::number(userID2) + " AND recvID=" + QString::number(userID1);
    QSqlQuery query = utls.researchi("message_table", conditions);

    return query;
}
