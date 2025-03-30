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
        Stone, Iron, Gold, Diamond, Ruby, Emerald, Lapiz, Topaz, Amethyst,
        // Radioactive
        Uranium, Thorium,
        // Tools
        StonePickaxe, IronPickaxe, DiamondPickaxe, RubyPickaxe, EmeraldPickaxe,
        // Weapons
        StoneSword, IronSword, DiamondSword, GemstoneSlicer, NuclearBlaster,
        // Chest
        Chest,
        // Entity
        Player, Serpent,
        // Count
        Count
    };

    enum Direction { North, East, South, West };

private:
    // Maxes
    static constexpr auto s_maxHealth { 30 };
    static constexpr auto s_maxHunger { 15 };

    // Table for converting the enum values into strings
    std::array<std::string_view, Item::Count> m_itemStringTable
    {
        "Air", "Wood", "Log", "Stick", "Apple", "Cracker", "Bread", "Potato", "Jacket potato", 
        "Carrot", "Mango", "Stone", "Iron", "Gold", "Diamond", "Ruby", "Emerald", "Lapiz", 
        "Topaz", "Amethyst", "Uranium", "Thorium", "Stone pickaxe", "Iron pickaxe", 
        "Diamond pickaxe", "Ruby pickaxe", "Emerald pickaxe", "Stone sword", "Iron sword",
        "Diamond sword", "Gemstone slicer", "Nuclear blaster", "Chest", "Player", "Serpent"
    };

    // Table for converting the enums into symbols
    std::array<std::string_view, Item::Count> m_itemSymbolTable
    {
        "\x1b[48;5;238m \x1b[0m", "Wood", "Log", "Stick", "Apple", "Cracker", "Bread", "Potato", 
        "Jacket potato", "Carrot", "Mango", "\x1b[48;5;236m \x1b[0m", "I", "\x1b[33mG\x1b[0m", "Diamond", "Ruby", 
        "Emerald", "Lapiz", "Topaz", "Amethyst", "Uranium", "Thorium", "Stone pickaxe", 
        "Iron pickaxe", "Diamond pickaxe", "Ruby pickaxe", "Emerald pickaxe", "Stone sword",
        "Iron sword", "Diamond sword", "Gemstone slicer", "Nuclear blaster", "C", "\x1b[1;32m☺︎\x1b[0m", "\x1b[31m§\x1b[0m"
    };

    // Cave and depth
    std::array<std::array<Item, 16>, 16> m_cave {};
    unsigned m_depth { 7 };
    // Player information
    unsigned m_x { 5 };
    unsigned m_y { 5 };
    std::unordered_map<Item, unsigned> m_inventory {};
    unsigned m_health { s_maxHealth };
    unsigned m_hunger { s_maxHunger };
    bool m_inSerpentFight {};
    // The number of turns from the start
    unsigned m_turns { 10 };

    // Random number generation
    std::random_device m_rndDevice {};
    std::mt19937 m_rng { m_rndDevice() };

public:
    Cavern();

    void Play();

private:
    // Command functions
    void GetPlayerCommand();
    void Move(Direction direction);
    void Mine();
    void Attack();
    void OpenChest();
    void Help();
    // Cave generation functions
    void GenerateCave();
    void GenerateSerpent();
    void GenerateMinerals();
    // Player functions
    void UpdatePlayer();
    // Printing functions
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
        GetPlayerCommand();
        UpdatePlayer();
    }
}

void Cavern::GetPlayerCommand()
{
    // Get the user input
    std::cout << "> ";
    std::string input;
    std::cin >> input;

    // Run command
    switch (std::tolower(input[0]))
    {
    case 'n': Move(Direction::North); break;
    case 's': Move(Direction::South); break;
    case 'w': Move(Direction::West); break;
    case 'e': Move(Direction::East); break;
    //case 'm': Mine(); break;
    /*case 'a': 
        Attack();
        break;
    case 'o': 
        OpenChest();
        break;
    case 'h': 
        Help();
        break;*/
    default:
        std::cout << "Bad command! use 'h' for help" << std::endl;
        GetPlayerCommand();
    }

    // Increase the amount of turns
    m_turns++;
}

void Cavern::Move(Direction direction)
{
    switch (direction)
    {
    case Direction::North: m_y -= m_y > 0 && m_cave[m_y - 1][m_x] == Item::Air; break;
    case Direction::South: 
        m_y += m_y < m_cave.size() - 1 && m_cave[m_y + 1][m_x] == Item::Air; break;
    case Direction::West: m_x -= m_x > 0 && m_cave[m_y][m_x - 1] == Item::Air; break;
    case Direction::East: 
        m_x += m_x < m_cave[m_y].size() - 1 && m_cave[m_y][m_x + 1] == Item::Air; break;
    default:
        break;
    }
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
        if (m_depth % 10 == 0)
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
    //m_cave[m_y][m_x] = Item::Player;
    m_inSerpentFight = true;
}

void Cavern::GenerateMinerals()
{
    // Generate iron at levels 0-3
    if (m_depth < 4)
    {
        std::uniform_int_distribution<int> ironDist(0, 30);
        for (int y{}; y < (int)m_cave.size(); y++)
            for (int x{}; x < (int)m_cave[y].size(); x++)
                if (ironDist(m_rng) == 30 && m_cave[y][x] == Item::Stone)
                    m_cave[y][x] = Item::Iron;
    }
    // Generate iron and gold at levels 4 - 7
    else if (m_depth < 8)
    {
        std::uniform_int_distribution<int> ironDist(0, 20);
        std::uniform_int_distribution<int> goldDist(0, 60);
        for (int y{}; y < (int)m_cave.size(); y++)
            for (int x{}; x < (int)m_cave[y].size(); x++)
                if (ironDist(m_rng) == 20 && m_cave[y][x] == Item::Stone)
                    m_cave[y][x] = Item::Iron;
                else if (goldDist(m_rng) == 50 && m_cave[y][x] == Item::Stone)
                    m_cave[y][x] = Item::Gold;
    }
    // gold + diamond
    //m_depth < 16
    // diamond + ruby
    //m_depth < 24
    // all
    //m_depth < 32
}

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
    for (unsigned y{}; y < m_cave.size(); y++)
    {
        for (unsigned x{}; x < m_cave[y].size(); x++)
            if (y == m_y && x == m_x)
                std::cout << m_itemSymbolTable[Item::Player];
            else
                std::cout << m_itemSymbolTable[m_cave[y][x]];
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
