#include <array>
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
        // Serpent
        Serpent,
        // Count
        Count
    };

private:
    // Maxes
    static constexpr auto s_maxHealth{ 30 };
    static constexpr auto s_maxHunger{ 15 };

    // Table for converting the enum values into strings
    std::array<std::string_view, Item::Count> m_itemStringTable
    {
        "-", "Wood", "Log", "Stick", "Apple", "Cracker", "Bread", "Potato", "Jacket potato", 
        "Carrot", "Mango", "#", "Coal", "Iron", "Gold", "Diamond", "Ruby", "Emerald", "Lapiz", 
        "Topaz", "Amethyst", "Uranium", "Thorium", "Stone pickaxe", "Iron pickaxe", 
        "Diamond pickaxe", "Ruby pickaxe", "Emerald pickaxe", "Stone sword", "Iron sword",
        "Diamond sword", "Gemstone slicer", "Nuclear blaster", "§"
    };

    // Cave and depth
    std::array<std::array<Item, 16>, 16> m_cave{};
    unsigned depth{};
    // Player information
    std::unordered_map<Item, unsigned> m_inventory{};
    unsigned m_health{ s_maxHealth };
    unsigned m_hunger{ s_maxHunger };
    // The number of turns from the start
    unsigned m_turns{};

public:
    Cavern();

private:
    void GenerateCave();
    void GenerateMonster();
    void GenerateMinerals();
    void PrintCave();
    void PrintPlayerInfo();
};

Cavern::Cavern()
{
    m_inventory[Item::Apple] = 32;
    m_inventory[Item::Uranium] = 17;
    m_inventory[Item::Stick] = 2;
    GenerateCave();
    PrintPlayerInfo();
    PrintCave();
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

        // Every 10 depth a monster will have to be defeated
        if (depth % 10 == 0)
            GenerateMonster();
        else
            GenerateMinerals();


        // Update the previous offset
        prevOff = off;
    }
}

void Cavern::GenerateMonster()
{

}

void Cavern::GenerateMinerals()
{}

void Cavern::PrintCave()
{
    for (const auto& row : m_cave)
    {
        for (const auto& col : row)
            std::print("{}", m_itemStringTable[col][0]);
        std::print("\n");
    }
}

void Cavern::PrintPlayerInfo()
{
    // Print the player stats
    std::println("Health: {}/{}", m_health, s_maxHealth);
    std::println("Hunger: {}/{}", m_hunger, s_maxHunger);
    // Print the inventory
    for (const auto&[item, amount] : m_inventory)
        std::println("{}x {}", amount, m_itemStringTable[item]);
}

int main()
{
    Cavern cavern;
}
