#include <array>
#include <iostream>
#include <print>
#include <random>
#include <string_view>
#include <unordered_map>

class Cavern
{
private:
    enum Item
    {
        // Nature 
        Air, Wood, Log, Stick,
        // Food
        Apple, Cracker, Bread, Potato, JacketPotato, Carrot, Mango,
        // Minerals
        Stone, Coal, Iron, Gold, Diamond, Ruby, Emerald, Lapiz, Topaz, Amethyst,
        // Radioactive
        Uranium, Thorium,
        // Tools
        StonePickaxe, IronPickaxe, DiamondPickaxe, RubyPickaxe, EmeraldPickaxe,
        // Weapons
        StoneSword, IronSword, DiamondSword, GemstoneSlicer, NuclearBlaster,
        // Entities
        Player, Serpent,
        // Count
        Count
    };

    enum Command
    {
        MoveNorth = 'n', MoveEast = 'e', MoveSouth = 's', MoveWest = 'w', Mine ='m', Attack = 'a',
        Open = 'o', Help = 'h'
    };

private:
    // Maxes
    static constexpr auto s_maxHealth{ 30 };
    static constexpr auto s_maxHunger{ 15 };

    // Table for converting the enum values into strings
    std::array<std::string_view, Item::Count> m_itemStringTable
    {
        "Air", "Wood", "Log", "Stick", "Apple", "Cracker", "Bread", "Potato", "Jacket potato", 
        "Carrot", "Mango", "Stone", "Coal", "Iron", "Gold", "Diamond", "Ruby", "Emerald", "Lapiz", 
        "Topaz", "Amethyst", "Uranium", "Thorium", "Stone pickaxe", "Iron pickaxe", 
        "Diamond pickaxe", "Ruby pickaxe", "Emerald pickaxe", "Stone sword", "Iron sword",
        "Diamond sword", "Gemstone slicer", "Nuclear blaster", "Player", "Serpent"
    };

    // Cave and depth
    std::array<std::array<Item, 16>, 16> m_cave{};
    unsigned depth{};
    // Player information
    unsigned m_x{};
    unsigned m_y{};
    std::unordered_map<Item, unsigned> m_inventory{};
    unsigned m_health{ s_maxHealth };
    unsigned m_hunger{ s_maxHunger };
    bool m_inSerpentFight{};
    // The number of turns from the start
    unsigned m_turns{ 10 };

public:
    Cavern();

    void Play();

private:
    void GetPlayerResponse();
    void GenerateCave();
    void GenerateSerpent();
    void GenerateMinerals();
    void UpdatePlayer();
    void PrintCave();
    void PrintPlayerInfo();
};

Cavern::Cavern()
{
    GenerateCave();
}

void Cavern::Play()
{
    while (true)
    {
        PrintCave();
        PrintPlayerInfo();
        GetPlayerResponse();
        UpdatePlayer();
    }
}

void Cavern::GetPlayerResponse()
{
    // Get the user input
    std::cout << "> ";
    std::string input;
    std::cin >> input;

    // Run command
    switch (std::tolower(input[0]))
    {
    case Command::MoveNorth: break;
    case Command::MoveSouth: break;
    case Command::MoveWest: break;
    case Command::MoveEast: break;
    case Command::Mine: break;
    case Command::Attack: break;
    case Command::Open: break;
    case Command::Help: break;
    default:
        std::cout << "Bad command! use 'h' for help" << std::endl;
        GetPlayerResponse();
    }

    // Increase the amount of turns
    m_turns++;
}

void Cavern::GenerateCave()
{
    // Create a random number generator and the various distributions needed
    std::random_device rndDevice;
    std::mt19937 rng(rndDevice());
    std::uniform_int_distribution<int> airSpaceDist(8, 9);
    std::uniform_int_distribution<int> offDist(-1, 2);

    // Generate an initial offset that is positive
    int prevOff{ offDist(rng) + 2 };

    for (auto& row : m_cave)
    {
        int rowSz = (int)row.size();

        // Fill the row of the cave with stone
        row.fill(Item::Stone);

        // Generate the air space and offset of it
        int airSpace{ airSpaceDist(rng) };
        int off{ prevOff + offDist(rng) };
        // Handle extreme values for offset
        off = off + airSpace > rowSz ? off - ((off + airSpace) - rowSz) - offDist(rng) : off;
        off = off < 1 ? 2 : off;

        // Fill in the air space
        for (int i{}; i < airSpace && i + off < rowSz - 1; i++)
            row[i + off] = Item::Air;

        // Every 10 depth a serpent will have to be defeated
        if (depth % 10 == 0)
            GenerateSerpent();
        else
            GenerateMinerals();


        // Update the previous offset
        prevOff = off;
    }
}

void Cavern::GenerateSerpent()
{
    m_cave[3][11] = Item::Serpent;
    m_cave[5][11] = Item::Player;
    m_inSerpentFight = true;
}

void Cavern::GenerateMinerals()
{}

void Cavern::UpdatePlayer()
{
    // Start to die if there is no hunger and every 12 turns the hunger goes down
    if (m_hunger == 0)
        m_health--;
    else if (m_turns % 12 == 0)
        m_hunger--;
}

void Cavern::PrintCave()
{
    for (const auto& row : m_cave)
    {
        for (const auto& col : row)
            switch (col)
            {
            case Item::Air:
                std::cout << "-";
                break;
            case Item::Stone:
                std::cout << "#";
                break;
            case Item::Player:
                std::cout << "\x1b[1;32m☺︎\x1b[0m";
                break;
            case Item::Serpent:
                std::cout << "\x1b[1;31m§\x1b[0m";
                break;
            default:
                std::cout << m_itemStringTable[col][0];
                break;
            }
        std::cout << std::endl;
    }
}

void Cavern::PrintPlayerInfo()
{
    std::cout << "Health: " << m_health << "/" << s_maxHealth << std::endl;
    std::cout << "Hunger: " << m_hunger << "/" << s_maxHunger << std::endl;
}

int main()
{
    Cavern cavern;
    cavern.Play();
}
