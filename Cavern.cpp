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

    enum Direction
    {
        North, East, South, West
    };

private:
    // Cave size
    static constexpr auto s_caveSz{ 16 };

    // Maxes
    static constexpr auto s_maxHealth{ 30 };
    static constexpr auto s_maxHunger{ 15 };

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
        "\x1b[48;5;238m \x1b[0m", "W", "L", "S", "Á", "C", "B", "P", "J", "C", "M", 
        "\x1b[48;5;236m \x1b[0m", "I", "\x1b[33mG\x1b[0m", "\x1b[36mD\x1b[0m", "\x1b[31mR\x1b[0m", 
        "\x1b[32mE\x1b[0m", "\x1b[34mL\x1b[0m", "\x1b[33mT\x1b[0m", "\x1b[35mA\x1b[0m", 
        "\x1b[42mU\x1b[0m", "\x1b[44mT\x1b[0m", "\x1b[38;5;238m¶\x1b[0m", "¶", "\x1b[36m¶\x1b[0m", 
        "\x1b[31m¶\x1b[0m", "\x1b[32m¶\x1b[0m", "\x1b[38;5;238m†\x1b[0m", "†", "\x1b[36m†\x1b[0m", 
        "\x1b[44m\x1b[35m†\x1b[0m", "\x1b[42m¬\x1b[0m", "\x1b[33mÇ\x1b[0m", "\x1b[1;32m☺︎\x1b[0m",
        "\x1b[31m§\x1b[0m"
    };

    // Cave and depth
    std::array<std::array<Item, s_caveSz>, s_caveSz> m_cave{};
    unsigned m_depth{ 51 };
    
    // Player information
    unsigned m_x{ 5 };
    unsigned m_y{ 5 };
    std::unordered_map<Item, unsigned> m_inventory{};
    unsigned m_health{ s_maxHealth };
    unsigned m_hunger{ s_maxHunger };
    bool m_inSerpentFight{};
    
    // The number of turns from the start
    unsigned m_turns{ 0 };

    // Random number generation
    std::random_device m_rndDevice{};
    std::mt19937 m_rng{ m_rndDevice() };

public:
    Cavern();

    void Play();

private:
    // Command functions
    void GetPlayerCommand();
    void Move(Direction direction);
    void Mine();
    void OpenChest();
    
    // Cave generation functions
    void GenerateCave();
    void GenerateSerpent();
    void GenerateMinerals();
    void GenerateChest();
    
    // Printing functions
    void PrintGame();
};

Cavern::Cavern()
{
    GenerateCave();
}

void Cavern::Play()
{
    while (true)
    {
        PrintGame();
        GetPlayerCommand();
    }
}

void Cavern::GetPlayerCommand()
{
    // Get the user input
    std::string input;
    std::cout << "\x1b[" << s_caveSz + 2 << "H> ";
    std::cin >> input;

    // Run command
    for (const auto& c : input) 
    {
        switch (std::tolower(c))
        {
        case 'n': Move(Direction::North); break;
        case 's': Move(Direction::South); break;
        case 'w': Move(Direction::West); break;
        case 'e': Move(Direction::East); break;
        case 'm': Mine(); break;
        };
    }

    // Increase the amount of turns
    m_turns++;
}

void Cavern::Move(Direction direction)
{
    switch (direction)
    {
    case Direction::North: 
        if (m_y > 0 && m_cave[m_y - 1][m_x] == Item::Air)
            m_y--; 
        break;
    case Direction::South: 
        if (m_y < s_caveSz - 1 && m_cave[m_y + 1][m_x] == Item::Air)
            m_y++;
        break;
    case Direction::West: 
        if (m_x > 0 && m_cave[m_y][m_x - 1] == Item::Air)
            m_x--;
        break;
    case Direction::East: 
        if (m_x < s_caveSz - 1 && m_cave[m_y][m_x + 1] == Item::Air)
            m_x++;
        break;
    }
}

void Cavern::Mine()
{
    // Mine the minerals around the player
    for (int y{ -1 }; y <= 1; y++)
    {
        if (m_y + y < 0 || m_y + y > s_caveSz)
            continue;

        for (int x{ -1 }; x <= 1; x++)
        {
            if (m_x + x < 0 || m_x + x > s_caveSz)
                continue;
            
            Item& col = m_cave[m_y + y][m_x + x];
            if (col != Item::Air && col != Item::Chest)
            {
                m_inventory[col]++;
                col = Item::Air;
            }
        }
    }
}

void Cavern::OpenChest()
{
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
        // Fill the row of the cave with stone
        row.fill(Item::Stone);

        // Generate the air space and offset of it
        int airSpace{ airSpaceDist(rng) };
        int off{ prevOff + offDist(rng) };

        // Handle extreme values for offset
        if (off + airSpace > s_caveSz)
            off -= ((off + airSpace) - s_caveSz) - offDist(rng);
        else if (off < 1)
            off = 2;

        // Fill in the air space
        for (int i{}; i < airSpace && i + off < s_caveSz - 1; i++)
            row[i + off] = Item::Air;

        // Generate the rest of the cave
        GenerateMinerals();
        GenerateChest();

        // Update the previous offset
        prevOff = off;
    }
}

void Cavern::GenerateSerpent()
{
}

void Cavern::GenerateMinerals()
{
    if (m_depth < 50)
    {
        // Defines that every 10 in depth the mineral will change
        constexpr int mineralChange = 10;

        // Random generators for the main and secondary mineral
        std::uniform_int_distribution<int> primaryDist(0, 29);
        std::uniform_int_distribution<int> secondaryDist(0, 59);

        // Calculate the primary and secondary mineral
        Item primary = static_cast<Item>(Item::Stone + m_depth / mineralChange);
        Item secondary = static_cast<Item>(Item::Iron + m_depth / mineralChange);
        
        // Generate the minerals
        for (auto& row : m_cave)
            for (auto& col : row)
                // 1 in 30 change of primary mineral
                if (primaryDist(m_rng) == primaryDist.max() && col == Item::Stone)
                    col = primary;
                // 1 in 60 chance of secondary mineral
                else if (secondaryDist(m_rng) == secondaryDist.max() && col == Item::Stone)
                    col = secondary;
    }
    // After a depth of 50 generate all minerals diamond and above
    else
    {
        // Random generators for the deciding stone or mineral and which mineral
        std::uniform_int_distribution<int> stoneDist(0, 7);
        std::uniform_int_distribution<int> mineralDist(Item::Diamond, Item::Thorium);
        
        // Generate the minerals
        for (auto& row : m_cave)
            for (auto& col : row)
                // 1 in 8 chance of a mineral
                if (stoneDist(m_rng) == stoneDist.max() && col == Item::Stone)
                    col = static_cast<Item>(mineralDist(m_rng));
    }
}

void Cavern::GenerateChest()
{
    // Genrate a chest randomly on all levels
    std::uniform_int_distribution<int> chestDist(0, 7);
    // 1 in 8 chance of a chest generating
    if (chestDist(m_rng) == chestDist.max())
        m_cave[7][7] = Item::Chest;
}

void Cavern::PrintGame()
{
    // Clear the screen
    std::cout << "\x1b[2J\x1b[1;1H";
    
    // Print the cave
    for (const auto& row : m_cave)
    {
        for (const auto& col : row)
            std::cout << m_itemSymbolTable[col];
        std::cout << std::endl;
    }

    // Print the player
    std::cout << "\x1b[" << m_y + 1 << ";" << m_x + 1 << "H" << m_itemSymbolTable[Item::Player];

    // Print player and cave information
    std::cout << "\x1b[2;19HHealth : " << m_health << "/" << s_maxHealth;
    std::cout << "\x1b[3;19HHunger : " << m_hunger << "/" << s_maxHunger;
    std::cout << "\x1b[4;19HDepth  : " << m_depth;

    // Print player inventory
    // TODO : clean this up
    int x{ 19 }, y{ 6 }, longest{ 0 };
    for (const auto&[ item, amount ]: m_inventory)
    {
        if (y == 16)
        {
            y = 6;
            x += longest;
            longest = 0;
        }
        std::string s = std::format("{}x {}", amount, m_itemStringTable[item]);
        if (s.size() > longest)
            longest = (int)s.size() + 2;
        std::cout << "\x1b[" << y++ << ";" << x << "H" << s; 
    }
}

int main()
{
    Cavern cavern;
    cavern.Play();
}
