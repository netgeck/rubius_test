/* 
 * File:   MsgPack_gtest.cxx
 * Author: kostya
 * 
 * Created on 15 Март 2016 г., 12:35
 */

#include <math.h>
#include <stdint.h>
#include <gtest/gtest.h>

#include "common.h"
#include "MsgPack_pack.h"
#include "MsgPack_unpack.h"
#include "../MsgPack/MsgPack_core/MsgPack.h"// header c приватной обл. видимости. Указываем путь


/// Тестовое число для проверки пакетов больших объемов данных (вплоть до UINT32_MAX)
#define UINT32_TESTNUM		(UINT16_MAX+1)
#define UINT16_TESTNUM		(UINT8_MAX+1)
#define UINT8_TESTNUM(x)	(x+1)

#define DIVISION_CEIL(x, y)	(1 + ((x - 1) / y))

const std::string testStr("test");


// NIL
TEST(NILL, nill) {
	MsgPack::package pkg = MsgPack::pack::nill();
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::nill(pkg), nullptr);
}

// BOOL
TEST(BOOLEAN, boolean) {
	MsgPack::package pkg;
	pkg = MsgPack::pack::boolean(true);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::boolean(pkg), true);
	
	pkg = MsgPack::pack::boolean(false);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::boolean(pkg), false);
}

// UNSIGNED INT. uint8_t
TEST(UINT, uint8){
	MsgPack::package pkg;
	uint8_t testNum = 0;
	pkg = MsgPack::pack::integer<uint8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(pkg), testNum);
	
	testNum = 127;
	pkg = MsgPack::pack::integer<uint8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(pkg), testNum);
	
	testNum = UINT8_MAX;
	pkg = MsgPack::pack::integer<uint8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(pkg), testNum);
}

// UNSIGNED INT. uint16_t
TEST(UINT, uint16){
	MsgPack::package pkg;
	uint16_t testNum = 0;
	pkg = MsgPack::pack::integer<uint16_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint16_t>(pkg), testNum);
	
	testNum = UINT16_MAX;
	pkg = MsgPack::pack::integer<uint16_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint16_t>(pkg), testNum);
}

// UNSIGNED INT. uint32_t
TEST(UINT, uint32){
	MsgPack::package pkg;
	uint32_t testNum = 0;
	pkg = MsgPack::pack::integer<uint32_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint32_t>(pkg), testNum);
	
	testNum = UINT32_MAX;
	pkg = MsgPack::pack::integer<uint32_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint32_t>(pkg), testNum);
}

// UNSIGNED INT. uint64_t
TEST(UINT, uint64){
	MsgPack::package pkg;
	uint64_t testNum = 0;
	pkg = MsgPack::pack::integer<uint64_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint64_t>(pkg), testNum);
	
	testNum = UINT64_MAX;
	pkg = MsgPack::pack::integer<uint64_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<uint64_t>(pkg), testNum);
}

// INT. int8_t
TEST(INT, int8){
	MsgPack::package pkg;
	int8_t testNum = 0;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
	
	testNum = -1;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
	
	testNum = -32;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
	
	testNum = -33;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
	
	testNum = INT8_MIN;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
	
	testNum = INT8_MAX;
	pkg = MsgPack::pack::integer<int8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int8_t>(pkg), testNum);
}

// INT. int16_t
TEST(INT, int16){
	MsgPack::package pkg;
	int16_t testNum = 0;
	pkg = MsgPack::pack::integer<int16_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int16_t>(pkg), testNum);
	
	testNum = INT16_MIN;
	pkg = MsgPack::pack::integer<int16_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int16_t>(pkg), testNum);
	
	testNum = INT16_MAX;
	pkg = MsgPack::pack::integer<int16_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int16_t>(pkg), testNum);
}

// INT. int32_t
TEST(INT, int32){
	MsgPack::package pkg;
	int32_t testNum = 0;
	pkg = MsgPack::pack::integer<int32_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int32_t>(pkg), testNum);
	
	testNum = INT32_MIN;
	pkg = MsgPack::pack::integer<int32_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int32_t>(pkg), testNum);
	
	testNum = INT32_MAX;
	pkg = MsgPack::pack::integer<int32_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int32_t>(pkg), testNum);
}

// INT. int64_t
TEST(INT, int64){
	MsgPack::package pkg;
	int64_t testNum = 0;
	pkg = MsgPack::pack::integer<int64_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int64_t>(pkg), testNum);
	
	testNum = INT64_MIN;
	pkg = MsgPack::pack::integer<int64_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int64_t>(pkg), testNum);
	
	testNum = INT64_MAX;
	pkg = MsgPack::pack::integer<int64_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::integer<int64_t>(pkg), testNum);
}

// FLOAT. float
TEST(FLOAT, float_32){
	MsgPack::package pkg;
	float testNum = 0;
	pkg = MsgPack::pack::float_<float>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::float_<float>(pkg), testNum);
	
	testNum = M_PI;
	pkg = MsgPack::pack::float_<float>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::float_<float>(pkg), testNum);
}

// FLOAT. double
TEST(FLOAT, float_64){
	MsgPack::package pkg;
	double testNum = 0;
	pkg = MsgPack::pack::float_<double>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::float_<double>(pkg), testNum);
	
	testNum = M_PI;
	pkg = MsgPack::pack::float_<double>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::float_<double>(pkg), testNum);
}

// STRING. str 8
TEST(STRING, length_uint8){
	MsgPack::package pkg;
	uint8_t testLength;
	std::string testStr;
	
	// fixstr
	testLength = 31;
	testStr.assign(testLength, '_');
	testStr.replace(0, 4, "test");
	pkg = MsgPack::pack::str(testStr);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::str(pkg), testStr);
	
	// str 8
	testLength = UINT8_MAX;
	testStr.assign(testLength, '*');
	testStr.replace(0, 4, "test");
	pkg = MsgPack::pack::str(testStr);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::str(pkg), testStr);
}

// STRING. str 16
TEST(STRING, length_uint16){
	uint16_t testLength = 31;
	std::string testStr(testLength, '#');
	testStr.replace(0, 4, "test");
	MsgPack::package pkg = MsgPack::pack::str(testStr);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::str(pkg), testStr);
}

// STRING. str 32
TEST(STRING, length_uint32){
	uint32_t testLength = UINT32_TESTNUM;
	std::string testStr(testLength, '!');
	testStr.replace(0, 4, "test");
	MsgPack::package pkg = MsgPack::pack::str(testStr);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::str(pkg), testStr);
}

// BIN. uint8_t
TEST(BINARY, length_uint8){
	uint8_t testLength = UINT8_MAX;
	MsgPack::rawData testBin(testLength, '#');
	MsgPack::package pkg = MsgPack::pack::bin(testBin);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::bin(pkg), testBin);
}

// BIN. uint16_t
TEST(BINARY, length_uint16){
	uint16_t testLength = UINT16_MAX;
	MsgPack::rawData testBin(testLength, '#');
	MsgPack::package pkg = MsgPack::pack::bin(testBin);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::bin(pkg), testBin);
}

// BIN. uint32_t
TEST(BINARY, length_uint32){
	uint32_t testLength = UINT32_TESTNUM;
	MsgPack::rawData testBin(testLength, '#');
	MsgPack::package pkg = MsgPack::pack::bin(testBin);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::bin(pkg), testBin);
}

// BIN. uint32_t
TEST(BINARY, interface_raw_ptr){
	uint8_t testLength = UINT8_MAX;
	MsgPack::rawData testBin(testLength, '#');
	MsgPack::package pkg = MsgPack::pack::bin(testBin.data(), testBin.size());
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::bin(pkg), testBin);
}

// Массив разнородных элементов. Проверяем работу с двумя элементами разного типа
TEST(ARRAY, length_uint8) {
	MsgPack::array_description array;
	array.push_back(MsgPack::pack::integer<uint8_t>(1));
	array.push_back(MsgPack::pack::str("test"));
	MsgPack::package pkg = MsgPack::pack::array(array);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::array(pkg), array);
	EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(array[0]), 1);
	EXPECT_EQ(MsgPack::unpack::str(array[1]), "test");
}

// Массив разнородных элементов. Работа с массивом N элементов, где N описывается типом uint16_t.
TEST(ARRAY, length_uint16) {
	MsgPack::array_description array;
	for (uint16_t i = 0; i <= UINT16_TESTNUM; i++){
		array.push_back(MsgPack::pack::integer<uint8_t>(0xF0));
	}
	MsgPack::package pkg = MsgPack::pack::array(array);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::array(pkg), array);
	for (uint16_t i = 0; i <= UINT16_TESTNUM; i++){
		EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(array[i]), 0xF0);
	}
}

// Массив разнородных элементов. Работа с массивом N элементов, где N описывается типом uint32_t.
TEST(ARRAY, length_uint32) {
	MsgPack::array_description array;
	for (uint32_t i = 0; i <= UINT32_TESTNUM; i++){
		array.push_back(MsgPack::pack::integer<uint8_t>(0xF0));
	}
	MsgPack::package pkg = MsgPack::pack::array(array);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::array(pkg), array);
	for (uint32_t i = 0; i <= UINT32_TESTNUM; i++){
		EXPECT_EQ(MsgPack::unpack::integer<uint8_t>(array[i]), 0xF0);
	}
}

// ARRAY сложные массивы. Тест массива элементом которого являетсья другой массив
TEST(ARRAY, complex) {
	MsgPack::array_description arraySimple;
	arraySimple.push_back(MsgPack::pack::integer<uint8_t>(1));
	arraySimple.push_back(MsgPack::pack::integer<uint8_t>(2));
	arraySimple.push_back(MsgPack::pack::integer<uint8_t>(3));
	
	MsgPack::array_description arrayNestedComplex;
	{
		MsgPack::array_description arraySimple_2;
		arraySimple_2.push_back(MsgPack::pack::integer<uint8_t>(1));
		arraySimple_2.push_back(MsgPack::pack::integer<uint8_t>(7));
		
		arrayNestedComplex.push_back(MsgPack::pack::array(arraySimple_2));
	}
	
	MsgPack::array_description arrayComplex;
	arrayComplex.push_back(MsgPack::pack::integer<uint8_t>(4));
	arrayComplex.push_back(MsgPack::pack::integer<uint8_t>(5));
	arrayComplex.push_back(MsgPack::pack::array(arraySimple));
	arrayComplex.push_back(MsgPack::pack::array(arrayNestedComplex));
	
	MsgPack::package pkg = MsgPack::pack::array(arrayComplex);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::array(pkg), arrayComplex);
}

// MAP
TEST(MAP, length_uint8) {
	MsgPack::map_description map_descript;
	map_descript.insert({MsgPack::pack::str("val"), MsgPack::pack::integer(7)});
	map_descript.insert({MsgPack::pack::str("text"), MsgPack::pack::str("value text")});
	
	MsgPack::package pkg = MsgPack::pack::map(map_descript);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::map(pkg), map_descript);
}

// MAP
TEST(MAP, length_uint16) {
	MsgPack::map_description map_descript;
	for (uint16_t i = 0; i < UINT16_MAX; i++){
		map_descript.insert({MsgPack::pack::str("val"), 
		MsgPack::pack::integer(i)});
	}
	
	MsgPack::package pkg = MsgPack::pack::map(map_descript);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::map(pkg), map_descript);
}

// MAP
TEST(MAP, length_uint32) {
	MsgPack::map_description map_descript;
	for (uint32_t i = 0; i < UINT32_TESTNUM; i++){
		map_descript.insert({MsgPack::pack::str("val"), 
		MsgPack::pack::integer(i)});
	}
	
	MsgPack::package pkg = MsgPack::pack::map(map_descript);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::map(pkg), map_descript);
}

// MAP	сложные map'ы
TEST(MAP, complex) {
	MsgPack::map_description map_descript_simple;
	map_descript_simple.insert(
		{MsgPack::pack::str("key"), MsgPack::pack::integer<uint8_t>(1)});
	MsgPack::map_description map_descript_complex;
	map_descript_complex.insert(
		{MsgPack::pack::str("Вложенная структура:"), MsgPack::pack::map(map_descript_simple)});	
	
	MsgPack::package pkg = MsgPack::pack::map(map_descript_complex);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	EXPECT_EQ(MsgPack::unpack::map(pkg), map_descript_complex);
}


// Случаи неправильного использования

// ERROR.	Некоректный заголовок
TEST(ERROR, wrong_header) {
	MsgPack::package incorrectPkg;
	incorrectPkg.push_back(0xc1);
	
	EXPECT_FALSE(MsgPack::isPgkCorrect(incorrectPkg));
}

// ERROR.	Попытка принудительной распаковки пакета с неправильно заданным именем.
TEST(ERROR, unpack_uint8_as_uint16){
	uint8_t testNum = UINT8_MAX;
	// Создаём пакет содержащий переменную uint8_t
	MsgPack::package pkg = MsgPack::pack::integer<uint8_t>(testNum);
	EXPECT_TRUE(MsgPack::isPgkCorrect(pkg));
	// Распаковываем пакет как содержащий uint16_t. Должно вылететь исключение
	EXPECT_THROW(MsgPack::unpack::integer<uint16_t>(pkg), std::runtime_error);
}

// ERROR.	Добавление в ассоциативный контейнер двух записей с одинаковыми ключами
TEST(ERROR, map_double_keys){
	MsgPack::map_description Map;
	Map.insert({MsgPack::pack::str("key"), MsgPack::pack::integer<uint8_t>(1)});
	Map.insert({MsgPack::pack::str("key"), MsgPack::pack::integer<uint8_t>(2)});

	// В контейнере должен остаться 1 элемент
	EXPECT_EQ(Map.size(), 1);
	// В контейнере содержится первый добавленый элемент
	EXPECT_EQ(Map.at(MsgPack::pack::str("key")), MsgPack::pack::integer<uint8_t>(1));
}


// Соответствие спецификации


// упакованное число должно быть приведено к big-endian
TEST(SPEC, endian_integer_uint16){
	uint16_t x_be = htobe16(0xFF00);
	uint16_t x_le = htole16(0xFF00);
	
	MsgPack::package pkg = MsgPack::pack::integer<uint16_t>(x_le);
	
	uint16_t actual(0);
	memcpy(&actual, &pkg[1], sizeof(uint16_t));
	
	EXPECT_EQ(actual, x_be);
}

