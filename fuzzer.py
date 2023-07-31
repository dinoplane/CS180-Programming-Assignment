import sys
import random

def roll(chances):
    sect = []
    val = 0
    for event, p in chances.items():
        val += p
        sect.append((event, val))
    
    # if (val != 1):
    #     print("WARNING: probablities don't add to 1")

    ret = ''
    roll = random.random()*val
    for event, s in sect:
        if (roll < s):
            ret = event
            break

    return ret

def main():
    if (len(sys.argv) < 3):
        print("Invalid args")
    else:
        file_num = int(sys.argv[1])
        n = int(sys.argv[2])

        chances = {
                    '0' : 0.6, 
                    '1' : 0.2,
                    '2' : 0.1,
                    '3' : 0.1
                  }

        for x in range(file_num):
            hp = random.randint(0, 20)
            with open(f"./tests/ali_test{x}.txt", "w") as f:
                f.write(f"{n} {hp}\n")
                for i in range(n):
                    for j in range(n):
                        if (i == 0 and j == 0):
                            continue

                        tile_type = roll(chances)
                        if (tile_type in '23'):
                            tile_val = 0
                        elif (tile_type == '1'):
                            tile_val = random.randint(0, 6)
                        else:
                            tile_val = random.randint(0, 20)
                            
                        f.write(f"{i} {j} {tile_type} {tile_val}\n")
                        
if __name__ == "__main__":
    main()