/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// HashTable.hpp - Header file for the HashTable class.
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HashTable.hpp

#pragma once

#include <memory_resource>
#include <shared_mutex>
#include <exception>
#include <optional>
#include <vector>
#include <mutex>
#include <ostream>
#include <concepts>

namespace DiscordCoreAPI {

	namespace DiscordCoreInternal {
		struct EventDelegateToken;
	}

	class EtfSerializer;

	template<typename ValueType>
	concept EtfSerializerT = std::same_as<ValueType, EtfSerializer>;

	template<typename ValueType>
	concept BoolT = std::same_as<std::decay_t<ValueType>, bool> && !
	EtfSerializerT<ValueType> && !std::integral<ValueType>;

	template<typename ValueType>
	concept EnumT = std::is_enum<std::decay_t<ValueType>>::value;

	template<typename ValueType>
	concept IntegerT = std::integral<std::decay_t<ValueType>> && !
	BoolT<std::decay_t<ValueType>>;

	class GuildMemberCacheData;
	struct VoiceStateDataLight;
	struct VoiceStateData;
	class GuildMemberData;

	template<typename ValueType>
	concept VoiceStateT = std::same_as<ValueType, VoiceStateDataLight>;

	template<typename ValueType>
	concept GuildMemberT = std::same_as<ValueType, GuildMemberCacheData> || std::same_as<ValueType, GuildMemberData>;

	template<typename ValueType>
	concept HasId = requires(ValueType value) { value.id; };

	template<typename ValueType>
	concept EventDelegateTokenT = std::same_as<ValueType, DiscordCoreInternal::EventDelegateToken>;

	struct ObjectCompare {
		template<typename ValueType01, typename ValueType02> inline bool operator()(const ValueType01& lhs, const ValueType02& rhs) const {
			return lhs == rhs;
		}

		template<typename ValueType01, typename ValueType02> inline bool operator()(ValueType01& lhs, ValueType02& rhs) {
			return lhs == rhs;
		}
	};

	struct KeyHasher {

		template<HasId ValueType> uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other.id.operator const uint64_t&(), sizeof(uint64_t));
		}

		template<EventDelegateTokenT ValueType> uint64_t operator()(const ValueType& other) const;

		template<uint64_t size> inline uint64_t operator()(const char (&other)[size]) const {
			return internalHashFunction(other, std::char_traits<char>::length(other));
		}

		template<typename ValueType> inline uint64_t operator()(const Jsonifier::StringBase<ValueType>& other) {
			return internalHashFunction(other.data(), other.size());
		}

		template<IntegerT ValueType> inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}

		template<EnumT ValueType> inline uint64_t operator()(const ValueType& other) const {
			return internalHashFunction(&other, sizeof(other));
		}

		inline uint64_t operator()(const std::string& other) const {
			return internalHashFunction(other.data(), other.size());
		}

		inline uint64_t operator()(const std::string_view& other) const {
			return internalHashFunction(other.data(), other.size());
		}

		inline uint64_t operator()(const std::vector<std::string>& data) const {
			std::string newString{};
			for (auto& value: data) {
				newString.append(value);
			}
			return internalHashFunction(newString.data(), newString.size());
		}

	  protected:
		inline uint64_t internalHashFunction(const void* value, uint64_t count) const {
			static constexpr uint64_t fnvOffsetBasis{ 0xcbf29ce484222325 };
			static constexpr uint64_t fnvPrime{ 0x00000100000001B3 };
			uint64_t hash{ fnvOffsetBasis };
			for (uint64_t x = 0; x < count; ++x) {
				hash ^= static_cast<const uint8_t*>(value)[x];
				hash *= fnvPrime;
			}
			return hash;
		}
	};

	template<typename ValueType> struct HashPolicy {
	  public:
		inline uint64_t indexForHash(uint64_t hash) const {
			return (hash & static_cast<const ValueType*>(this)->capacityVal - 1);
		}

		inline uint64_t nextSizeOver(uint64_t size) const {
			size = static_cast<uint64_t>(HashPolicy::nextSizeOverPrime(size));
			--size;
			size |= size >> 1;
			size |= size >> 2;
			size |= size >> 4;
			size |= size >> 8;
			size |= size >> 16;
			size |= size >> 32;
			++size;
			return size;
		}

	  protected:
		enum class PrimeNumbers : uint64_t {
			Prime0 = 0llu,
			Prime2 = 2llu,
			Prime3 = 3llu,
			Prime5 = 5llu,
			Prime7 = 7llu,
			Prime11 = 11llu,
			Prime13 = 13llu,
			Prime17 = 17llu,
			Prime23 = 23llu,
			Prime29 = 29llu,
			Prime37 = 37llu,
			Prime47 = 47llu,
			Prime59 = 59llu,
			Prime73 = 73llu,
			Prime97 = 97llu,
			Prime127 = 127llu,
			Prime151 = 151llu,
			Prime197 = 197llu,
			Prime251 = 251llu,
			Prime313 = 313llu,
			Prime397 = 397llu,
			Prime499 = 499llu,
			Prime631 = 631llu,
			Prime797 = 797llu,
			Prime1009 = 1009llu,
			Prime1259 = 1259llu,
			Prime1597 = 1597llu,
			Prime2011 = 2011llu,
			Prime2539 = 2539llu,
			Prime3203 = 3203llu,
			Prime4027 = 4027llu,
			Prime5087 = 5087llu,
			Prime6421 = 6421llu,
			Prime8089 = 8089llu,
			Prime10193 = 10193llu,
			Prime12853 = 12853llu,
			Prime16193 = 16193llu,
			Prime20399 = 20399llu,
			Prime25717 = 25717llu,
			Prime32401 = 32401llu,
			Prime40823 = 40823llu,
			Prime51437 = 51437llu,
			Prime64811 = 64811llu,
			Prime81649 = 81649llu,
			Prime102877 = 102877llu,
			Prime129607 = 129607llu,
			Prime163307 = 163307llu,
			Prime205759 = 205759llu,
			Prime259229 = 259229llu,
			Prime326617 = 326617llu,
			Prime411527 = 411527llu,
			Prime518509 = 518509llu,
			Prime653267 = 653267llu,
			Prime823117 = 823117llu,
			Prime1037059 = 1037059llu,
			Prime1306601 = 1306601llu,
			Prime1646237 = 1646237llu,
			Prime2074129 = 2074129llu,
			Prime2613229 = 2613229llu,
			Prime3292489 = 3292489llu,
			Prime4148279 = 4148279llu,
			Prime5226491 = 5226491llu,
			Prime6584983 = 6584983llu,
			Prime8296553 = 8296553llu,
			Prime10453007 = 10453007llu,
			Prime13169977 = 13169977llu,
			Prime16593127 = 16593127llu,
			Prime20906033 = 20906033llu,
			Prime26339969 = 26339969llu,
			Prime33186281 = 33186281llu,
			Prime41812097 = 41812097llu,
			Prime52679969 = 52679969llu,
			Prime66372617 = 66372617llu,
			Prime83624237 = 83624237llu,
			Prime105359939 = 105359939llu,
			Prime132745199 = 132745199llu,
			Prime167248483 = 167248483llu,
			Prime210719881 = 210719881llu,
			Prime265490441 = 265490441llu,
			Prime334496971 = 334496971llu,
			Prime421439783 = 421439783llu,
			Prime530980861 = 530980861llu,
			Prime668993977 = 668993977llu,
			Prime842879579 = 842879579llu,
			Prime1061961721 = 1061961721llu,
			Prime1337987929 = 1337987929llu,
			Prime1685759167 = 1685759167llu,
			Prime2123923447 = 2123923447llu,
			Prime2675975881 = 2675975881llu,
			Prime3371518343 = 3371518343llu,
			Prime4247846927 = 4247846927llu,
			Prime5351951779 = 5351951779llu,
			Prime6743036717 = 6743036717llu,
			Prime8495693897 = 8495693897llu,
			Prime10703903591 = 10703903591llu,
			Prime13486073473 = 13486073473llu,
			Prime16991387857 = 16991387857llu,
			Prime21407807219 = 21407807219llu,
			Prime26972146961 = 26972146961llu,
			Prime33982775741 = 33982775741llu,
			Prime42815614441 = 42815614441llu,
			Prime53944293929 = 53944293929llu,
			Prime67965551447 = 67965551447llu,
			Prime85631228929 = 85631228929llu,
			Prime107888587883 = 107888587883llu,
			Prime135931102921 = 135931102921llu,
			Prime171262457903 = 171262457903llu,
			Prime215777175787 = 215777175787llu,
			Prime271862205833 = 271862205833llu,
			Prime342524915839 = 342524915839llu,
			Prime431554351609 = 431554351609llu,
			Prime543724411781 = 543724411781llu,
			Prime685049831731 = 685049831731llu,
			Prime863108703229 = 863108703229llu,
			Prime1087448823553 = 1087448823553llu,
			Prime1370099663459 = 1370099663459llu,
			Prime1726217406467 = 1726217406467llu,
			Prime2174897647073 = 2174897647073llu,
			Prime2740199326961 = 2740199326961llu,
			Prime3452434812973 = 3452434812973llu,
			Prime4349795294267 = 4349795294267llu,
			Prime5480398654009 = 5480398654009llu,
			Prime6904869625999 = 6904869625999llu,
			Prime8699590588571 = 8699590588571llu,
			Prime10960797308051 = 10960797308051llu,
			Prime13809739252051 = 13809739252051llu,
			Prime17399181177241 = 17399181177241llu,
			Prime21921594616111 = 21921594616111llu,
			Prime27619478504183 = 27619478504183llu,
			Prime34798362354533 = 34798362354533llu,
			Prime43843189232363 = 43843189232363llu,
			Prime55238957008387 = 55238957008387llu,
			Prime69596724709081 = 69596724709081llu,
			Prime87686378464759 = 87686378464759llu,
			Prime110477914016779 = 110477914016779llu,
			Prime139193449418173 = 139193449418173llu,
			Prime175372756929481 = 175372756929481llu,
			Prime220955828033581 = 220955828033581llu,
			Prime278386898836457 = 278386898836457llu,
			Prime350745513859007 = 350745513859007llu,
			Prime441911656067171 = 441911656067171llu,
			Prime556773797672909 = 556773797672909llu,
			Prime701491027718027 = 701491027718027llu,
			Prime883823312134381 = 883823312134381llu,
			Prime1113547595345903 = 1113547595345903llu,
			Prime1402982055436147 = 1402982055436147llu,
			Prime1767646624268779 = 1767646624268779llu,
			Prime2227095190691797 = 2227095190691797llu,
			Prime2805964110872297 = 2805964110872297llu,
			Prime3535293248537579 = 3535293248537579llu,
			Prime4454190381383713 = 4454190381383713llu,
			Prime5611928221744609 = 5611928221744609llu,
			Prime7070586497075177 = 7070586497075177llu,
			Prime8908380762767489 = 8908380762767489llu,
			Prime11223856443489329 = 11223856443489329llu,
			Prime14141172994150357 = 14141172994150357llu,
			Prime17816761525534927 = 17816761525534927llu,
			Prime22447712886978529 = 22447712886978529llu,
			Prime28282345988300791 = 28282345988300791llu,
			Prime35633523051069991 = 35633523051069991llu,
			Prime44895425773957261 = 44895425773957261llu,
			Prime56564691976601587 = 56564691976601587llu,
			Prime71267046102139967 = 71267046102139967llu,
			Prime89790851547914507 = 89790851547914507llu,
			Prime113129383953203213 = 113129383953203213llu,
			Prime142534092204280003 = 142534092204280003llu,
			Prime179581703095829107 = 179581703095829107llu,
			Prime226258767906406483 = 226258767906406483llu,
			Prime285068184408560057 = 285068184408560057llu,
			Prime359163406191658253 = 359163406191658253llu,
			Prime452517535812813007 = 452517535812813007llu,
			Prime570136368817120201 = 570136368817120201llu,
			Prime718326812383316683 = 718326812383316683llu,
			Prime905035071625626043 = 905035071625626043llu,
			Prime1140272737634240411 = 1140272737634240411llu,
			Prime1436653624766633509 = 1436653624766633509llu,
			Prime1810070143251252131 = 1810070143251252131llu,
			Prime2280545475268481167 = 2280545475268481167llu,
			Prime2873307249533267101 = 2873307249533267101llu,
			Prime3620140286502504283 = 3620140286502504283llu,
			Prime4561090950536962147 = 4561090950536962147llu,
			Prime5746614499066534157 = 5746614499066534157llu,
			Prime7240280573005008577 = 7240280573005008577llu,
			Prime9122181901073924329 = 9122181901073924329llu,
			Prime11493228998133068689 = 11493228998133068689llu,
			Prime14480561146010017169 = 14480561146010017169llu,
			Prime18446744073709551557 = 18446744073709551557ll
		};

		inline static PrimeNumbers nextSizeOverPrime(size_t& size) {
			// prime numbers generated by the following method:
			// 1. start with a prime p = 2
			// 2. go to wolfram alpha and get p = NextPrime(2 * p)
			// 3. repeat 2. until you overflow 64 bits
			// you now have large gaps which you would hit if somebody called reserve() with an unlucky number.
			// 4. to fill the gaps for every prime p go to wolfram alpha and get ClosestPrime(p * 2^(1/3)) and ClosestPrime(p * 2^(2/3)) and put those in the gaps
			// 5. get PrevPrime(2^64) and put it at the end
			static constexpr const size_t prime_list[]{ 2llu, 3llu, 5llu, 7llu, 11llu, 13llu, 17llu, 23llu, 29llu, 37llu, 47llu, 59llu, 73llu, 97llu,
				127llu, 151llu, 197llu, 251llu, 313llu, 397llu, 499llu, 631llu, 797llu, 1009llu, 1259llu, 1597llu, 2011llu, 2539llu, 3203llu, 4027llu,
				5087llu, 6421llu, 8089llu, 10193llu, 12853llu, 16193llu, 20399llu, 25717llu, 32401llu, 40823llu, 51437llu, 64811llu, 81649llu,
				102877llu, 129607llu, 163307llu, 205759llu, 259229llu, 326617llu, 411527llu, 518509llu, 653267llu, 823117llu, 1037059llu, 1306601llu,
				1646237llu, 2074129llu, 2613229llu, 3292489llu, 4148279llu, 5226491llu, 6584983llu, 8296553llu, 10453007llu, 13169977llu, 16593127llu,
				20906033llu, 26339969llu, 33186281llu, 41812097llu, 52679969llu, 66372617llu, 83624237llu, 105359939llu, 132745199llu, 167248483llu,
				210719881llu, 265490441llu, 334496971llu, 421439783llu, 530980861llu, 668993977llu, 842879579llu, 1061961721llu, 1337987929llu,
				1685759167llu, 2123923447llu, 2675975881llu, 3371518343llu, 4247846927llu, 5351951779llu, 6743036717llu, 8495693897llu,
				10703903591llu, 13486073473llu, 16991387857llu, 21407807219llu, 26972146961llu, 33982775741llu, 42815614441llu, 53944293929llu,
				67965551447llu, 85631228929llu, 107888587883llu, 135931102921llu, 171262457903llu, 215777175787llu, 271862205833llu, 342524915839llu,
				431554351609llu, 543724411781llu, 685049831731llu, 863108703229llu, 1087448823553llu, 1370099663459llu, 1726217406467llu,
				2174897647073llu, 2740199326961llu, 3452434812973llu, 4349795294267llu, 5480398654009llu, 6904869625999llu, 8699590588571llu,
				10960797308051llu, 13809739252051llu, 17399181177241llu, 21921594616111llu, 27619478504183llu, 34798362354533llu, 43843189232363llu,
				55238957008387llu, 69596724709081llu, 87686378464759llu, 110477914016779llu, 139193449418173llu, 175372756929481llu,
				220955828033581llu, 278386898836457llu, 350745513859007llu, 441911656067171llu, 556773797672909llu, 701491027718027llu,
				883823312134381llu, 1113547595345903llu, 1402982055436147llu, 1767646624268779llu, 2227095190691797llu, 2805964110872297llu,
				3535293248537579llu, 4454190381383713llu, 5611928221744609llu, 7070586497075177llu, 8908380762767489llu, 11223856443489329llu,
				14141172994150357llu, 17816761525534927llu, 22447712886978529llu, 28282345988300791llu, 35633523051069991llu, 44895425773957261llu,
				56564691976601587llu, 71267046102139967llu, 89790851547914507llu, 113129383953203213llu, 142534092204280003llu, 179581703095829107llu,
				226258767906406483llu, 285068184408560057llu, 359163406191658253llu, 452517535812813007llu, 570136368817120201llu,
				718326812383316683llu, 905035071625626043llu, 1140272737634240411llu, 1436653624766633509llu, 1810070143251252131llu,
				2280545475268481167llu, 2873307249533267101llu, 3620140286502504283llu, 4561090950536962147llu, 5746614499066534157llu,
				7240280573005008577llu, 9122181901073924329llu, 11493228998133068689llu, 14480561146010017169llu, 18446744073709551557llu };
			static constexpr PrimeNumbers primeNumbers[]{ PrimeNumbers::Prime0, PrimeNumbers::Prime2, PrimeNumbers::Prime3, PrimeNumbers::Prime5,
				PrimeNumbers::Prime7, PrimeNumbers::Prime11, PrimeNumbers::Prime13, PrimeNumbers::Prime17, PrimeNumbers::Prime23,
				PrimeNumbers::Prime29, PrimeNumbers::Prime37, PrimeNumbers::Prime47, PrimeNumbers::Prime59, PrimeNumbers::Prime73,
				PrimeNumbers::Prime97, PrimeNumbers::Prime127, PrimeNumbers::Prime151, PrimeNumbers::Prime197, PrimeNumbers::Prime251,
				PrimeNumbers::Prime313, PrimeNumbers::Prime397, PrimeNumbers::Prime499, PrimeNumbers::Prime631, PrimeNumbers::Prime797,
				PrimeNumbers::Prime1009, PrimeNumbers::Prime1259, PrimeNumbers::Prime1597, PrimeNumbers::Prime2011, PrimeNumbers::Prime2539,
				PrimeNumbers::Prime3203, PrimeNumbers::Prime4027, PrimeNumbers::Prime5087, PrimeNumbers::Prime6421, PrimeNumbers::Prime8089,
				PrimeNumbers::Prime10193, PrimeNumbers::Prime12853, PrimeNumbers::Prime16193, PrimeNumbers::Prime20399, PrimeNumbers::Prime25717,
				PrimeNumbers::Prime32401, PrimeNumbers::Prime40823, PrimeNumbers::Prime51437, PrimeNumbers::Prime64811, PrimeNumbers::Prime81649,
				PrimeNumbers::Prime102877, PrimeNumbers::Prime129607, PrimeNumbers::Prime163307, PrimeNumbers::Prime205759, PrimeNumbers::Prime259229,
				PrimeNumbers::Prime326617, PrimeNumbers::Prime411527, PrimeNumbers::Prime518509, PrimeNumbers::Prime653267, PrimeNumbers::Prime823117,
				PrimeNumbers::Prime1037059, PrimeNumbers::Prime1306601, PrimeNumbers::Prime1646237, PrimeNumbers::Prime2074129,
				PrimeNumbers::Prime2613229, PrimeNumbers::Prime3292489, PrimeNumbers::Prime4148279, PrimeNumbers::Prime5226491,
				PrimeNumbers::Prime6584983, PrimeNumbers::Prime8296553, PrimeNumbers::Prime10453007, PrimeNumbers::Prime13169977,
				PrimeNumbers::Prime16593127, PrimeNumbers::Prime20906033, PrimeNumbers::Prime26339969, PrimeNumbers::Prime33186281,
				PrimeNumbers::Prime41812097, PrimeNumbers::Prime52679969, PrimeNumbers::Prime66372617, PrimeNumbers::Prime83624237,
				PrimeNumbers::Prime105359939, PrimeNumbers::Prime132745199, PrimeNumbers::Prime167248483, PrimeNumbers::Prime210719881,
				PrimeNumbers::Prime265490441, PrimeNumbers::Prime334496971, PrimeNumbers::Prime421439783, PrimeNumbers::Prime530980861,
				PrimeNumbers::Prime668993977, PrimeNumbers::Prime842879579, PrimeNumbers::Prime1061961721, PrimeNumbers::Prime1337987929,
				PrimeNumbers::Prime1685759167, PrimeNumbers::Prime2123923447, PrimeNumbers::Prime2675975881, PrimeNumbers::Prime3371518343,
				PrimeNumbers::Prime4247846927, PrimeNumbers::Prime5351951779, PrimeNumbers::Prime6743036717, PrimeNumbers::Prime8495693897,
				PrimeNumbers::Prime10703903591, PrimeNumbers::Prime13486073473, PrimeNumbers::Prime16991387857, PrimeNumbers::Prime21407807219,
				PrimeNumbers::Prime26972146961, PrimeNumbers::Prime33982775741, PrimeNumbers::Prime42815614441, PrimeNumbers::Prime53944293929,
				PrimeNumbers::Prime67965551447, PrimeNumbers::Prime85631228929, PrimeNumbers::Prime107888587883, PrimeNumbers::Prime135931102921,
				PrimeNumbers::Prime171262457903, PrimeNumbers::Prime215777175787, PrimeNumbers::Prime271862205833, PrimeNumbers::Prime342524915839,
				PrimeNumbers::Prime431554351609, PrimeNumbers::Prime543724411781, PrimeNumbers::Prime685049831731, PrimeNumbers::Prime863108703229,
				PrimeNumbers::Prime1087448823553, PrimeNumbers::Prime1370099663459, PrimeNumbers::Prime1726217406467,
				PrimeNumbers::Prime2174897647073, PrimeNumbers::Prime2740199326961, PrimeNumbers::Prime3452434812973,
				PrimeNumbers::Prime4349795294267, PrimeNumbers::Prime5480398654009, PrimeNumbers::Prime6904869625999,
				PrimeNumbers::Prime8699590588571, PrimeNumbers::Prime10960797308051, PrimeNumbers::Prime13809739252051,
				PrimeNumbers::Prime17399181177241, PrimeNumbers::Prime21921594616111, PrimeNumbers::Prime27619478504183,
				PrimeNumbers::Prime34798362354533, PrimeNumbers::Prime43843189232363, PrimeNumbers::Prime55238957008387,
				PrimeNumbers::Prime69596724709081, PrimeNumbers::Prime87686378464759, PrimeNumbers::Prime110477914016779,
				PrimeNumbers::Prime139193449418173, PrimeNumbers::Prime175372756929481, PrimeNumbers::Prime220955828033581,
				PrimeNumbers::Prime278386898836457, PrimeNumbers::Prime350745513859007, PrimeNumbers::Prime441911656067171,
				PrimeNumbers::Prime556773797672909, PrimeNumbers::Prime701491027718027, PrimeNumbers::Prime883823312134381,
				PrimeNumbers::Prime1113547595345903, PrimeNumbers::Prime1402982055436147, PrimeNumbers::Prime1767646624268779,
				PrimeNumbers::Prime2227095190691797, PrimeNumbers::Prime2805964110872297, PrimeNumbers::Prime3535293248537579,
				PrimeNumbers::Prime4454190381383713, PrimeNumbers::Prime5611928221744609, PrimeNumbers::Prime7070586497075177,
				PrimeNumbers::Prime8908380762767489, PrimeNumbers::Prime11223856443489329, PrimeNumbers::Prime14141172994150357,
				PrimeNumbers::Prime17816761525534927, PrimeNumbers::Prime22447712886978529, PrimeNumbers::Prime28282345988300791,
				PrimeNumbers::Prime35633523051069991, PrimeNumbers::Prime44895425773957261, PrimeNumbers::Prime56564691976601587,
				PrimeNumbers::Prime71267046102139967, PrimeNumbers::Prime89790851547914507, PrimeNumbers::Prime113129383953203213,
				PrimeNumbers::Prime142534092204280003, PrimeNumbers::Prime179581703095829107, PrimeNumbers::Prime226258767906406483,
				PrimeNumbers::Prime285068184408560057, PrimeNumbers::Prime359163406191658253, PrimeNumbers::Prime452517535812813007,
				PrimeNumbers::Prime570136368817120201, PrimeNumbers::Prime718326812383316683, PrimeNumbers::Prime905035071625626043,
				PrimeNumbers::Prime1140272737634240411, PrimeNumbers::Prime1436653624766633509, PrimeNumbers::Prime1810070143251252131,
				PrimeNumbers::Prime2280545475268481167, PrimeNumbers::Prime2873307249533267101, PrimeNumbers::Prime3620140286502504283,
				PrimeNumbers::Prime4561090950536962147, PrimeNumbers::Prime5746614499066534157, PrimeNumbers::Prime7240280573005008577,
				PrimeNumbers::Prime9122181901073924329, PrimeNumbers::Prime11493228998133068689, PrimeNumbers::Prime14480561146010017169,
				PrimeNumbers::Prime18446744073709551557 };
			const size_t* found = std::lower_bound(std::begin(prime_list), std::end(prime_list) - 1, size);
			size = *found;
			return primeNumbers[1 + found - prime_list];
		}
	};

	template<typename FirstType, typename SecondType> class Pair {
	  public:
		using first_type = FirstType;
		using second_type = SecondType;

		first_type first;
		second_type second;

		template<typename FirstTypeNew, typename SecondTypeNew> inline Pair(FirstTypeNew&& firstNew, SecondTypeNew&& secondNew)
			: first{ std::forward<FirstTypeNew>(firstNew) }, second{ std::forward<SecondTypeNew>(secondNew) } {};

		template<typename SecondTypeNew> inline Pair& operator=(SecondTypeNew&& secondNew) {
			second = std::forward<SecondTypeNew>(secondNew);
			return *this;
		};

		inline bool operator==(const Pair& other) const {
			return this->first == other.first && second == other.second;
		}
	};

	template<typename ValueTypeInternal> class HashIterator {
	  public:
		using iterator_category = std::forward_iterator_tag;
		using value_type_internal = ValueTypeInternal;
		using value_type = ValueTypeInternal::value_type;
		using reference = value_type&;
		using pointer = value_type*;
		using const_reference = const value_type&;
		using const_pointer = const value_type*;
		using pointer_internal = value_type_internal*;
		using size_type = uint64_t;

		inline HashIterator() noexcept = default;

		inline HashIterator(const pointer_internal valueNew) : value{ valueNew } {
			if (value) {
				skipEmptySlots();
			}
		};

		inline const HashIterator& initializeIterator(const pointer_internal valueNew) const {
			value = valueNew;
			if (value) {
				skipEmptySlots();
			}
			return *this;
		}

		inline HashIterator& initializeIterator(pointer_internal valueNew) {
			value = valueNew;
			if (value) {
				skipEmptySlots();
			}
			return *this;
		}

		inline HashIterator& operator++() {
			++value;
			skipEmptySlots();
			return *this;
		}

		inline pointer_internal getRawPtr() {
			return value;
		}

		inline bool operator==(const HashIterator&) const {
			return value->areWeDone();
		}

		inline const_pointer operator->() const {
			return &value->value;
		}

		inline const_reference operator*() const {
			return value->value;
		}

		inline pointer operator->() {
			return &value->value;
		}

		inline reference operator*() {
			return value->value;
		}

		inline ~HashIterator(){};

	  protected:
		mutable pointer_internal value;

		inline void skipEmptySlots() const {
			while (value->areWeEmpty() && !value->areWeDone()) {
				value++;
			};
		}
	};

	template<typename ValueType> struct ObjectCore {
		using value_type = ValueType;

		union {
			value_type value;
		};
		int8_t currentIndex{ 0 };

		inline ObjectCore(int8_t newIndex) : currentIndex{ newIndex } {};

		inline ObjectCore() : currentIndex{ 0 } {};

		template<typename... Args> inline void enable(Args&&... valueNew) {
			if (currentIndex > 0) {
				value.~value_type();
			}
			new (std::addressof(value)) value_type{ std::forward<Args>(valueNew)... };
			currentIndex = 1;
		}

		inline bool areWeActive() const {
			return currentIndex > 0;
		}

		inline bool areWeEmpty() const {
			return currentIndex == 0;
		}

		inline bool areWeDone() const {
			return currentIndex == -1;
		}

		inline void disable() {
			if (currentIndex > 0) {
				value.~value_type();
			}
			currentIndex = 0;
		}

		inline bool operator==(const ObjectCore& other) const {
			return value == other.value;
		}

		inline ~ObjectCore() {
			if (currentIndex > 0) {
				value.~value_type();
			}
		}
	};

	template<typename KeyType, typename ValueType> class UnorderedMap;

	template<typename MapIterator, typename KeyType, typename ValueType>
	concept MapContainerIteratorT = std::is_same_v<typename UnorderedMap<KeyType, ValueType>::iterator, std::decay_t<MapIterator>>;

	template<typename KeyType, typename ValueType> class UnorderedMap
		: protected HashPolicy<UnorderedMap<KeyType, ValueType>>,
		  protected JsonifierInternal::AllocWrapper<ObjectCore<Pair<KeyType, ValueType>>>,
		  protected ObjectCompare,
		  protected KeyHasher {
	  public:
		using mapped_type = ValueType;
		using key_type = KeyType;
		using reference = mapped_type&;
		using value_type = Pair<key_type, mapped_type>;
		using value_type_internal = ObjectCore<Pair<key_type, mapped_type>>;
		using const_reference = const mapped_type&;
		using size_type = uint64_t;
		using key_hasher = KeyHasher;
		using pointer = value_type_internal*;
		using object_compare = ObjectCompare;
		using hash_policy = HashPolicy<UnorderedMap<key_type, mapped_type>>;
		friend hash_policy;

		using iterator = HashIterator<value_type_internal>;
		friend iterator;
		using const_iterator = const HashIterator<value_type_internal>;
		friend const_iterator;

		inline static constexpr int8_t minimumLookups{ 4 };
		inline static int8_t currentMaxLookupDistance{ minimumLookups };

		using allocator = JsonifierInternal::AllocWrapper<value_type_internal>;

		class LocalIterator {
		  public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = typename value_type_internal::value_type;
			using reference = value_type&;
			using pointer = value_type*;
			using const_reference = const value_type&;
			using const_pointer = const value_type*;
			using pointer_internal = value_type_internal*;
			using size_type = uint64_t;

			inline constexpr LocalIterator(value_type_internal* valueNew) : currentValue{ valueNew }, startValue{ valueNew } {};

			inline constexpr LocalIterator& operator++() {
				++currentValue;
				skipEmptySlots();
				return *this;
			}

			inline constexpr operator iterator() {
				return currentValue;
			}

			inline pointer_internal getRawPtr() {
				return currentValue;
			}

			inline constexpr bool operator==(const LocalIterator&) const {
				return currentValue - startValue >= currentMaxLookupDistance;
			}

			inline constexpr const_pointer operator->() const {
				return &currentValue->value;
			}

			inline constexpr const_reference operator*() const {
				return currentValue->value;
			}

			inline constexpr pointer operator->() {
				return &currentValue->value;
			}

			inline constexpr reference operator*() {
				return currentValue->value;
			}

			inline constexpr ~LocalIterator(){};

		  protected:
			mutable pointer_internal currentValue{};
			mutable pointer_internal startValue{};

			inline constexpr void skipEmptySlots() const {
				while (currentValue->areWeEmpty() && !currentValue->areWeDone()) {
					currentValue++;
				};
			}
		};

		inline UnorderedMap(size_type capacityNew = 16) {
			reserve(capacityNew);
		};

		inline UnorderedMap& operator=(UnorderedMap&& other) noexcept {
			if (this != &other) {
				clear();
				swap(other);
			}
			return *this;
		}

		inline UnorderedMap(UnorderedMap&& other) noexcept {
			*this = std::move(other);
		}

		inline UnorderedMap& operator=(const UnorderedMap& other) {
			if (this != &other) {
				clear();

				reserve(other.capacity());
				for (const auto& [key, value]: other) {
					emplace(key, value);
				}
			}
			return *this;
		}

		inline UnorderedMap(const UnorderedMap& other) {
			*this = other;
		}

		inline UnorderedMap(std::initializer_list<value_type> list) {
			reserve(list.size());
			for (auto& value: list) {
				emplace(std::move(value.first), std::move(value.second));
			}
		};

		template<typename key_type_new, typename... Args> inline iterator emplace(key_type_new&& key, Args&&... value) {
			pointer currentEntry = data + hash_policy::indexForHash(key_hasher()(key));
			for (size_type x{}; x < currentMaxLookupDistance; ++x, ++currentEntry) {
				if (currentEntry->areWeEmpty()) {
					currentEntry->enable(std::forward<key_type_new>(key), std::forward<Args>(value)...);
					sizeVal++;
					return currentEntry;
				} else if (currentEntry->areWeActive() && object_compare()(currentEntry->value.first, key)) {
					currentEntry->value.second.~mapped_type();
					currentEntry->value = mapped_type{ std::forward<Args>(value)... };
					return currentEntry;
				}
			}
			resize(capacityVal + capacityVal + 2);
			return emplace(std::forward<key_type_new>(key), std::forward<Args>(value)...);
		}

		template<typename key_type_new> inline const_iterator find(key_type_new&& key) const {
			if (capacityVal > 0) {
				LocalIterator currentEntry{ data + hash_policy::indexForHash(key_hasher()(key)) };
				for (; currentEntry != currentEntry; ++currentEntry) {
					if (object_compare()(currentEntry->first, key)) {
						return currentEntry;
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator find(key_type_new&& key) {
			if (capacityVal > 0) {
				LocalIterator currentEntry{ data + hash_policy::indexForHash(key_hasher()(key)) };
				for (; currentEntry != currentEntry; ++currentEntry) {
					if (object_compare()(currentEntry->first, key)) {
						return currentEntry;
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline const_reference operator[](key_type_new&& key) const {
			return emplace(std::move(key), mapped_type())->second;
		}

		template<typename key_type_new> inline reference operator[](key_type_new&& key) {
			return emplace(std::move(key), mapped_type())->second;
		}

		template<typename key_type_new> inline const_reference at(key_type_new&& key) const {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw std::exception{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_new> inline reference at(key_type_new&& key) {
			auto iter = find(std::forward<key_type_new>(key));
			if (iter == end()) {
				throw std::exception{ "Sorry, but an object by that key doesn't exist in this map." };
			}
			return iter->second;
		}

		template<typename key_type_new> inline bool contains(key_type_new&& key) const {
			if (capacityVal > 0) {
				LocalIterator currentEntry{ data + hash_policy::indexForHash(key_hasher()(key)) };
				for (; currentEntry != currentEntry; ++currentEntry) {
					if (object_compare()(currentEntry->first, key)) {
						return true;
					}
				}
			}
			return false;
		}

		template<MapContainerIteratorT<key_type, mapped_type> MapIterator> inline iterator erase(MapIterator&& iter) {
			if (capacityVal > 0) {
				LocalIterator currentEntry{ data + static_cast<size_type>(iter.getRawPtr() - data) };
				for (; currentEntry != currentEntry; ++currentEntry) {
					if (object_compare()(currentEntry->first, iter.operator*().first)) {
						currentEntry.getRawPtr()->disable();
						sizeVal--;
						return ++currentEntry;
					}
				}
			}
			return end();
		}

		template<typename key_type_new> inline iterator erase(key_type_new&& key) {
			if (capacityVal > 0) {
				LocalIterator currentEntry{ data + hash_policy::indexForHash(key_hasher()(key)) };
				for (; currentEntry != currentEntry; ++currentEntry) {
					if (object_compare()(currentEntry->first, key)) {
						currentEntry.getRawPtr()->disable();
						sizeVal--;
						return ++currentEntry;
					}
				}
			}
			return end();
		}

		inline const_iterator begin() const {
			return const_iterator{ data };
		}

		inline const_iterator end() const {
			return {};
		}

		inline iterator begin() {
			return iterator{ data };
		}

		inline iterator end() {
			return {};
		}

		inline bool full() const {
			return static_cast<float>(sizeVal) >= static_cast<float>(capacityVal) * 0.90f;
		}

		inline size_type size() const {
			return sizeVal;
		}

		inline bool empty() const {
			return sizeVal == 0;
		}

		inline void reserve(size_type sizeNew) {
			sizeNew = sizeNew == 0 ? 4 : sizeNew;
			resize(sizeNew);
		}

		inline void swap(UnorderedMap& other) noexcept {
			std::swap(capacityVal, other.capacityVal);
			std::swap(sizeVal, other.sizeVal);
			std::swap(data, other.data);
		}

		inline size_type capacity() const {
			return capacityVal;
		}

		inline bool operator==(const UnorderedMap& other) const {
			if (capacityVal != other.capacityVal || sizeVal != other.sizeVal || data != other.data) {
				return false;
			}
			for (auto iter01{ begin() }, iter02{ other.begin() }; iter01 != end(); ++iter01, ++iter02) {
				if (!object_compare()(iter01.operator*().second, iter02.operator*().second)) {
					return false;
				}
			}
			return true;
		}

		inline void clear() {
			if (data && capacityVal > 0) {
				std::destroy(data, data + capacityVal);
				allocator::deallocate(data, capacityVal);
				sizeVal = 0;
				capacityVal = 0;
				data = nullptr;
			}
		}

		inline ~UnorderedMap() {
			clear();
		};

	  protected:
		value_type_internal* data{};
		size_type capacityVal{};
		size_type sizeVal{};

		inline static constexpr int8_t endValue{ -1 };

		inline static int8_t log2(size_t value) {
			static constexpr int8_t table[64] = { 63, 0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61, 51, 37, 40, 49, 18, 28, 20, 55, 30,
				34, 11, 43, 14, 22, 4, 62, 57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56, 45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8,
				23, 7, 6, 5 };
			value |= value >> 1;
			value |= value >> 2;
			value |= value >> 4;
			value |= value >> 8;
			value |= value >> 16;
			value |= value >> 32;
			return table[((value - (value >> 1)) * 8788775746428827150) >> 58];
		}

		inline static int8_t computeMaxLookupDistance(size_t numBuckets) {
			auto desired = log2(numBuckets);
			return std::max(int8_t{ 4 }, desired);
		}

		inline void resize(size_type capacityNew) {
			auto newSize = hash_policy::nextSizeOver(capacityNew);
			if (newSize > capacityVal) {
				auto oldPtr = data;
				auto oldCapacity = capacityVal;
				auto oldSize = sizeVal;
				sizeVal = 0;
				currentMaxLookupDistance = computeMaxLookupDistance(newSize + 1 + currentMaxLookupDistance);
				data = allocator::allocate(newSize + 1 + currentMaxLookupDistance);
				std::memset(data, 0, sizeof(value_type_internal) * (newSize + 1 + currentMaxLookupDistance));
				capacityVal = newSize;
				new (data + capacityVal) value_type_internal{ endValue };
				auto currentPtr = oldPtr;
				for (size_type x = 0; x < oldSize; ++currentPtr) {
					if (currentPtr->areWeActive()) {
						++x;
						emplace(std::move(currentPtr->value.first), std::move(currentPtr->value.second));
					}
				}
				if (oldPtr && oldCapacity) {
					allocator::deallocate(oldPtr, oldCapacity + 1 + currentMaxLookupDistance);
				}
			}
		}
	};
}
