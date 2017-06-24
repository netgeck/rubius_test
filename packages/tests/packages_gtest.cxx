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
	streamPkg pkg;
	EXPECT_FALSE(pkg.isFull());
}

TEST(package, fill_n_unpack) {
	std::string word("test");
	std::string fileContent("test test test");
	std::vector<char> file(fileContent.begin(), fileContent.end());
	
	streamPkg pkg;
	streamPkgFill(pkg, word.begin(), word.end(), file.begin(), file.end());
	
	ASSERT_GT(pkg.size(), sizeof(streamPkg::header));
	ASSERT_TRUE(pkg.isFull()) 
		<< "размер пакета: " << pkg.size()
		<< " в заголовке пакета: " << *reinterpret_cast<streamPkg::header*>(&pkg.m_buffer[0]);
	EXPECT_EQ(word, std::string(word_begin(pkg), word_end(pkg)));
	EXPECT_EQ(file, std::vector<char>(file_begin(pkg), file_end(pkg)));
}