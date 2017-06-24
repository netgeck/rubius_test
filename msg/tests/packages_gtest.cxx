/* 
 * File:   MsgPack_gtest.cxx
 * Author: kostya
 * 
 * Created on 15 Март 2016 г., 12:35
 */

#include <gtest/gtest.h>

#include <vector>
#include <streamPkg.h>


TEST(package, empty) {
	msg::package pkg;
	EXPECT_FALSE(pkg.isFull());
}

TEST(package, request_pack_unpack) {
	std::string word("test");
	std::string fileContent("test test test");
	std::vector<char> file(fileContent.begin(), fileContent.end());
	
	msg::package pkg;
	msg::request::packageFill(pkg, word.begin(), word.end(), file.begin(), file.end());
	
	ASSERT_GT(pkg.size(), sizeof(msg::package::header));
	ASSERT_TRUE(pkg.isFull()) 
		<< "размер пакета: " << pkg.size()
		<< " в заголовке пакета: " << *reinterpret_cast<msg::package::header*>(&pkg.m_buffer[0]);
	EXPECT_EQ(word, std::string(msg::request::word_begin(pkg), msg::request::word_end(pkg)));
	EXPECT_EQ(file, std::vector<char>(msg::request::file_begin(pkg), msg::request::file_end(pkg)));
}

TEST(package, answer_pack_unpack) {
	msg::answer::value num = 42;
	
	msg::package pkg;
	msg::answer::packageFill(pkg, num);
	
	ASSERT_GT(pkg.size(), sizeof(msg::package::header));
	ASSERT_TRUE(pkg.isFull()) 
		<< "размер пакета: " << pkg.size()
		<< " в заголовке пакета: " << *reinterpret_cast<msg::package::header*>(&pkg.m_buffer[0]);
	EXPECT_EQ(num, msg::answer::getNum(pkg));
}