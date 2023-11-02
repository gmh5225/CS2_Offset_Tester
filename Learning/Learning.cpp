#include <iostream>
#include <thread>

#include "memory.h"

namespace offsets {
    constexpr ::std::ptrdiff_t dwEntityList = 0x17995C0;
    constexpr ::std::ptrdiff_t m_iPawnHealth = 0x32C;
    constexpr ::std::ptrdiff_t m_sSanitizedPlayerName = 0x720;
    constexpr ::std::ptrdiff_t m_iTeamNum = 0x3BF;
    constexpr ::std::ptrdiff_t m_hPlayerPawn = 0x7BC;
}

int main()
{
    while (true) {

        uintptr_t entity_list = memory::memRead<uintptr_t>(memory::baseAddress + offsets::dwEntityList);
        if (!entity_list) {
            std::cout << "Entity list Not Found\n";
            break;
        }

        uintptr_t list_entry = memory::memRead<uintptr_t>(entity_list + 0x10);
        if (!list_entry) {
            std::cout << "List Entry Not Found\n";
            break;
        }

        for (int i = 1; i < 32; i++)
        {
            const uintptr_t entity = memory::memRead<uintptr_t>(list_entry + 120 * (i & 0x1FF));
            if (!entity)
                continue;

            const std::uint32_t entity_pawn = memory::memRead<std::uint32_t>(entity + offsets::m_hPlayerPawn);

            const uintptr_t list_entry2 = memory::memRead<uintptr_t>(entity_list + 0x8 * ((entity_pawn & 0x7FFF) >> 9) + 16);
            if (!list_entry2)
                continue;

            const uintptr_t pawn = memory::memRead<uintptr_t>(list_entry2 + 120 * (entity_pawn & 0x1FF));
            if (!pawn)
                continue;

            const int team_num = memory::memRead<int>(entity + offsets::m_iTeamNum);

            std::string entity_name = "Invalid Name";
            const DWORD64 name_address = memory::memRead<DWORD64>(entity + offsets::m_sSanitizedPlayerName);
            if (name_address) {
                char buf[256];
                memory::read_raw(name_address, buf, sizeof(buf));
                entity_name = std::string(buf);
            }

            const int pawn_health = memory::memRead<int>(pawn + offsets::m_iPawnHealth);
         
            std::cout << entity_name << " Team #" << team_num << "\n";

            if (pawn_health <= 0) {
                std::cout << "Dead" << "\n\n";
                continue;
            }
            std::cout << "  -> health " << pawn_health << "\n\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        system("CLS");

    }
    return 0;
}

