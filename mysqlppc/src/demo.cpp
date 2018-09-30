/*
 * =====================================================================================
 *
 *       Filename:  demo.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年10月20日 09时27分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wooce (Wooce Yang), wooce@21cn.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <iostream>
#include <memory>
#include "mysqlppc.h"

int main() {
  try {
    mysqlppc::Connection conn("localhost", 3316, "root", "", "root");
    std::auto_ptr<mysqlppc::Statement> stmt(
        conn.prepare("select * from student where name=?"));
    // stmt->bindIntParam(0,1);
    stmt->bindStrParam(0, "yang");
    stmt->execute();
    std::auto_ptr<mysqlppc::ResultSet> resultSet(stmt->getResultSet());
    while (resultSet->next()) {
      std::cout << "id:" << resultSet->getInt("id")
                << " name:" << resultSet->getString("name")
                << " regtime:" << resultSet->getString("regtime")
                << " salary:" << resultSet->getFloat("salary") << std::endl;
      resultSet->getInt(8);
    }
  } catch (mysqlppc::MysqlException &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
