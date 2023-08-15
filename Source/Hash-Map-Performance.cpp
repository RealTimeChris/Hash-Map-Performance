// Hash-Map-Performance.cpp : Defines the entry point for the application.
//

#include <Hash-Map-Performance.hpp>
#include <nanobench.h>

using namespace std;
class testStruct{
public:
testStruct() noexcept = default;
testStruct& operator=(testStruct&&) noexcept = default;
testStruct(testStruct&&) noexcept = default;
testStruct& operator=(const testStruct& other) noexcept = default;
testStruct(const testStruct& other) noexcept = default;
testStruct(std::string&& stringNew) {
	testString = std::move(stringNew);
}
operator std::string& () {
	return testString;
}
std::string testString{};
int32_t testInt{};
bool operator==(const testStruct& other) const {
	return testInt == other.testInt && testFloat == other.testFloat && testString == other.testString;
}
float testFloat{};
~testStruct() {
}
};

template<typename TimeType, typename Function> void benchmark(Function function, int64_t iterationCount, std::string_view benchmarkName) {
	TimeType currentLowestTime{};
	for (int64_t x = 0; x < iterationCount; ++x) {
		auto startTime = std::chrono::duration_cast<TimeType>(std::chrono::high_resolution_clock::now().time_since_epoch());
		function();
		auto endTime = std::chrono::duration_cast<TimeType>(std::chrono::high_resolution_clock::now().time_since_epoch());
		auto newTime = endTime - startTime;
		if (static_cast<double>(newTime.count()) < currentLowestTime) {
			currentLowestTime = newTime.count();
		}
	}std::cout << "Benchmark: " << benchmarkName << " Completed in: " << currentLowestTime << std::endl;
}

static constexpr int8_t maxProbingDistance{ 4 };
/*
template<typename ValueTypeInternal, typename ValueType> class CoreIterator {
  public:
	using iterator_category = std::forward_iterator_tag;
	using value_type_internal = ValueTypeInternal;
	using value_type = ValueType;
	using reference = value_type&;
	using pointer = value_type*;
	using const_reference = const value_type&;
	using const_pointer = const value_type*;
	using pointer_internal = value_type_internal*;
	using size_type = uint64_t;

	inline CoreIterator(const pointer_internal valueNew) : value(valueNew) {
		skipEmptySlots();
	};

	inline CoreIterator& operator=(CoreIterator&& other) noexcept {
		value = other.value;
		return *this;
	}

	inline CoreIterator(CoreIterator&& other) noexcept {
		*this = std::move(other);
	}

	inline CoreIterator& operator++() {
		skipEmptySlots();
		return *this;
	}

	inline pointer_internal getRawPtr() {
		return value;
	}

	inline bool operator==(const CoreIterator&) const {
		return value->areWeDone();
	}

	inline const_pointer operator->() const {
		return &value->getCurrentValue(currentIndex);
	}

	inline const_reference operator*() const {
		return value->getCurrentValue(currentIndex);
	}

	inline pointer operator->() {
		return &value->getCurrentValue(currentIndex);
	}

	inline reference operator*() {
		return value->getCurrentValue(currentIndex);
	}

	inline ~CoreIterator(){};

  protected:
	pointer_internal value;
	int8_t currentIndex{};
	int64_t currentGlobalIndex{};

	void skipEmptySlots() {
		if (currentIndex < value->currentIndex - 1) {
			++currentIndex;
		} else {
			do {
				value++;
			} while (value->areWeEmpty());
			currentIndex = 0;
		}
	}
};

template<typename ValueType> struct ObjectBucketList {
  public:
	inline ObjectBucketList() noexcept {};

	inline ObjectBucketList(ObjectBucketList* prevPtrNew, int8_t newValue) : currentIndex{ newValue }, prevPtr{ prevPtrNew } {};

	template<typename... Args> inline void enable(Args&&... args) {
		if (currentIndex < maxProbingDistance) {
			new (&arrayVal) ValueType{ std::forward<Args>(args)... };
		}
		currentIndex = 1;
	}

	inline void allocateNextNode() {
		nextPtr = std::make_unique<ObjectBucketList>(currentIndex + 1);
	}

	inline void disable() {
		if (currentIndex > 0) {
			arrayVal.~ValueType();
		}
	}

	ObjectBucketList* getNextPtr(int8_t desiredIndex) {
		if (desiredIndex > currentIndex + 1) {
			return nextPtr->getNextPtr(currentIndex - 1);
		} else if (desiredIndex < currentIndex) {
			return prevPtr->getNextPtr(currentIndex + 1);
		} else {
			return this;
		}
	}

	inline bool areWeDone() const {
		return currentIndex == std::numeric_limits<int8_t>::min();
	}

	inline bool areWeActive() const {
		return currentIndex > -1;
	}

	inline bool doWeHaveSpace() const {
		return currentIndex < maxProbingDistance;
	}

	inline bool areWeEmpty() const {
		return currentIndex == 0;
	}

	inline auto begin() {
		return CoreIterator<ObjectBucketList<ValueType>, ValueType>{ arrayVal };
	}

	inline auto end() {
		return CoreIterator<ObjectBucketList<ValueType>, ValueType>{ arrayVal + (currentIndex <= 0 ? 0 : currentIndex - 1) };
	}

	inline ~ObjectBucketList() {
		if (currentIndex > -1) {
			arrayVal.~ValueType();
		}
	}

	union {
		ValueType arrayVal;
	};
	std::unique_ptr<ObjectBucketList> nextPtr{};
	ObjectBucketList* prevPtr{};
	int8_t currentIndex{ 0 };
};

class TempClass {
  public:

	inline TempClass() {
		for (uint64_t x = 0; x < 4096; ++x) {
			values.emplace_back();
			int32_t currentIndex{};
			while (values.back().doWeHaveSpace()) {
				values.back().enable(x);
				++currentIndex;
				if (currentIndex > 1) {
					break;
				}
			}
			
		}
		values.back().currentIndex = std::numeric_limits<int8_t>::min();
	}
	std::vector<ObjectBucketList<uint64_t>> values{};
	std::vector<CoreIterator<ObjectBucketList<uint64_t>, uint64_t>> iterators{};
	auto begin() {
		return CoreIterator<ObjectBucketList<uint64_t>, uint64_t>{ values.data() };
	}
	auto end() {
		return CoreIterator<ObjectBucketList<uint64_t>, uint64_t>{ values.data() + values.size() };

	}

};
*/
// Define your LocalIterator types
struct LocalIterator1 {
	// Implementation for LocalIterator1
};

struct LocalIterator2 {
	// Implementation for LocalIterator2
};

struct LocalIterator3 {
	// Implementation for LocalIterator3
};

class LocalIteratorCollection {
  public:
	constexpr static std::size_t maxMaxLookupDistance = 3;// Adjust as needed

	LocalIteratorCollection() {
		// Initialize your LocalIterator instances here
		// For demonstration purposes, using the same instance for all types
		for (std::size_t i = 0; i < maxMaxLookupDistance; ++i) {
			iterators_[i] = LocalIterator1();
		}
	}

	// Accessor using [] operator
	template<std::size_t Index> decltype(auto) operator[](std::integral_constant<std::size_t, Index>) {
		if constexpr (Index < maxMaxLookupDistance) {
			return iterators_[Index];
		} else {
			throw std::out_of_range("Index out of bounds");
		}
	}

  private:
	using LocalIteratorVariant = std::variant<LocalIterator1, LocalIterator2, LocalIterator3>;
	std::array<LocalIteratorVariant, maxMaxLookupDistance> iterators_;
};

template<typename Test, template<typename...> class Ref> struct IsSpecializationV : std::false_type {};

template<template<typename...> class Ref, typename... Args> struct IsSpecializationV<Ref<Args...>, Ref> : std::true_type {};

template<typename ValueType>
concept VectorT = IsSpecializationV<ValueType, Jsonifier::Vector>::value;

template<VectorT ValueType> void functionTest(const ValueType&S) {
}

int main() {
	Jsonifier::Vector<int32_t> vector{};
	functionTest(vector);

	// Print something to avoid compiler optimization removing the code

	//TempClass tempObject{};

	//DiscordCoreAPI::UnorderedMap<std::string, std::string> mapTest{};
	//DiscordCoreAPI::UnorderedMap<std::string, std::string> mapTest02{ mapTest };
	
	int64_t result{}; 
	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, AIO Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
	}});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, Reserve Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, Emplacing Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>,[] operator Emplacing Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03[std::to_string(x)] = testStruct{ std::to_string(x) };
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, Iteration Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, Find Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("std::unordered_map<uint64_t, testStruct>, Erase Test", [&] {
		std::unordered_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
		}
		});
	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, AIO Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
		}});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, Reserve Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		map03.reserve(2048);
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, Emplacing Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>,[] operator Emplacing Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03[std::to_string(x)] = testStruct{ std::to_string(x) };
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, Iteration Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, Find Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("DiscordCoreAPI::UnorderedMap<uint64_t, testStruct>, Erase Test", [&] {
		DiscordCoreAPI::UnorderedMap<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
		}
		});
	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, AIO Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
		}});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, Reserve Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, Emplacing Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>,[] operator Emplacing Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		for (uint64_t x = 0; x < 4096; ++x) {
			map03[std::to_string(x)] = testStruct{ std::to_string(x) };
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, Iteration Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(iter.operator->()->first);
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, Find Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end(); ++iter) {
			result += stoull(map03.find(iter.operator*().first).operator*().second.operator std::string & ());
		}
		});

	ankerl::nanobench::Bench().epochs(10).epochIterations(100).run("flat_hash_map<uint64_t, testStruct>, Erase Test", [&] {
		flat_hash_map<std::string, testStruct> map03{};
		map03.reserve(2048);
		for (uint64_t x = 0; x < 4096; ++x) {
			map03.emplace(std::to_string(x), testStruct{ std::to_string(x) });
		}
		for (auto iter = map03.begin(); iter != map03.end();) {
			iter = map03.erase(iter);
		}
		});

	return 0;

}

