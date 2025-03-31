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
        "\x1b[48;5;238m \x1b[0m", "W", "L", "S", "Á", "C", "B", "P", "J", "C", "M", 
        "\x1b[48;5;236m \x1b[0m", "I", "\x1b[33mG\x1b[0m", "\x1b[36mD\x1b[0m", "\x1b[31mR\x1b[0m", 
        "\x1b[32mE\x1b[0m", "\x1b[34mL\x1b[0m", "\x1b[33mT\x1b[0m", "\x1b[35mA\x1b[0m", 
        "\x1b[42mU\x1b[0m", "\x1b[44mT\x1b[0m", "\x1b[38;5;238m¶\x1b[0m", "¶", "\x1b[36m¶\x1b[0m", 
        "\x1b[31m¶\x1b[0m", "\x1b[32m¶\x1b[0m", "\x1b[38;5;238m†\x1b[0m", "†", "\x1b[36m†\x1b[0m", 
        "\x1b[44m\x1b[35m†\x1b[0m", "\x1b[42m¬\x1b[0m", "C", "\x1b[1;32m☺︎\x1b[0m",
        "\x1b[31m§\x1b[0m"
    };

    // Cave and depth
    std::array<std::array<Item, 16>, 16> m_cave{};
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
    void Attack();
    void OpenChest();
    void Help();
    
    // Cave generation functions
    void GenerateCave();
    void GenerateSerpent();
    void GenerateMinerals();
    
    // Printing functions
    void PrintGame();
};

Cavern::Cavern()
{
    GenerateCave();
}

void Cavern::Play()
{
    m_inventory[Item::StonePickaxe] = 1;
    while (true)
    {
        PrintGame();
        GetPlayerCommand();
    }
}

void Cavern::GetPlayerCommand()
{
    // Get the user input
    std::cout << "\x1b[18;2H> ";
    std::string input;
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
    case Direction::North: m_y -= m_y > 0 && m_cave[m_y - 1][m_x] == Item::Air; break;
    case Direction::South: m_y += m_y < m_cave.size() - 1 && m_cave[m_y + 1][m_x] == Item::Air; break;
    case Direction::West: m_x -= m_x > 0 && m_cave[m_y][m_x - 1] == Item::Air; break;
    case Direction::East: m_x += m_x < m_cave[m_y].size() - 1 && m_cave[m_y][m_x + 1] == Item::Air; break;
    }
}

void Cavern::Mine()
{
    // Mine the minerals around the player
    for (int y{ -1 }; y <= 1; y++)
    {
        if (m_y + y < 0 || m_y + y > m_cave.size())
            continue;

        for (int x{ -1 }; x <= 1; x++)
        {
            if (m_x + x < 0 || m_x + x > m_cave[y].size())
                continue;

            m_inventory[m_cave[m_y + y][m_x + x]] += m_cave[m_y + y][m_x + x] != Item::Air;
            m_cave[m_y + y][m_x + x] = Item::Air;
        }
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
        #define rowSz static_cast<int>(row.size())

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

void Cavern::PrintGame()
{
    // Clear the screen
    std::cout << "\x1b[2J\x1b[1;1H";
    
    // Print the cave
    for (unsigned y{}; y < m_cave.size(); y++)
    {
        for (unsigned x{}; x < m_cave[y].size(); x++)
            // If the player is at the same position print it instead
            if (y == m_y && x == m_x)
                std::cout << m_itemSymbolTable[Item::Player];
            else
                std::cout << m_itemSymbolTable[m_cave[y][x]];
        std::cout << std::endl;
    }

    // Print player and cave information
    std::cout << "\x1b[2;19HHealth : " << m_health << "/" << s_maxHealth;
    std::cout << "\x1b[3;19HHunger : " << m_hunger << "/" << s_maxHunger;
    std::cout << "\x1b[4;19HDepth  : " << m_depth;
}

int main()
{
    Cavern cavern;
    cavern.Play();
}
