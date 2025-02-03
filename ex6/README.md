# Pokedex Manager 3000

> **"Gotta store 'em all (in a BST)!"** – Some random Oak guy

Welcome to **Pokedex Manager 3000**, a thrilling console-based system for:

- Creating your own Pokémon trainers (Owners),
- Assigning them a majestic BST (Binary Search Tree) of Pokémon,
- Doing epic fights, evolutions, merges, or just lazing around with your fuzzy data structures.

## Features That'll Blow Your Mind

- **New Pokedex Creation**  
  Because what is life without a brand-new Bulbasaur or Charmander?

- **Fight!**  
  Let your creatures settle scores. Or watch them tie. It's all about the 1.5×Attack + 1.2×HP, baby!

- **Evolutions**  
  We keep it simple: ID + 1 is the next step. Magic? Possibly. But who are we to question Pokémon logic?

- **Merging**  
  Two owners walk into a bar; only one walks out –– with both Pokedexes combined. The other is “mysteriously” gone afterward.

- **Circular Linked List**  
  Because life is a circle. Also because we want you to practice. You can loop around and around the owners like a carnival ride.

## Getting Started

1. **Compile**  
   Use your favorite modern or ancient build tools. Something like:
   gcc -Wall -Wextra -Werror -g -std=c99 ex6.c -o ex6
   Or pray to the compiler gods that everything runs.

2. **Run**  
valgrind ./ex6 < input.txt

Then follow the prompts. 
- Enter owners’ names,
- Insert Pokémon,
- Evolve them or make them brawl in the ring,
- Or do a fancy merge and watch an entire owner vanish from existence!

3. **Exit**  
At any time, use the “Exit” option. The program will say a final goodbye. Possibly weeping in the background.

## FAQ (Fancifully Asked Questions)

**Q: Where did my second owner go after merging?**  
A: The second owner is very tired from the BFS copying process and took a permanent nap. No tears, only data structure serenity.

**Q: What if I have leftover \r from ancient times?**  
A: Fear not, we have trimmed them! Or at least, we have a cunning plan to handle CRLF. 

**Q: Why can’t I see the source code magic?**  
A: This project is about learning. We want you to focus on your own approach and watch out for BST illusions and pointers.  
   But if you really want, here is a youtube video that solves the excercise step by step: https://www.youtube.com/watch?v=dQw4w9WgXcQ&ab

**Q: Does the code leak memory?**  
A: We sure tried our best.

## Final Notes
- **Disclaimer**: No real Pokémon were harmed.  
- **Pro Tip**: Don’t feed your Bulbasaur after midnight. 
- **Have Fun**: Tinker with merges, do fights, test extremes. Explore the labyrinth of pointers!

That’s all, folks. Jump in, compile, and see if you can **destroy** your data structures in ways we never imagined. Good luck!

