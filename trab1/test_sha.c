#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sha256.h"

int main(void) {
    const int NUM_PHRASES = 3;
    char *frases[] = {"hello world",
        "",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas efficitur nisl eu nisl facilisis tempus quis et neque. In hac habitasse platea dictumst. Duis sit amet nisi ex. Proin eu aliquam nibh. Duis interdum pellentesque nisi a venenatis. Nulla pulvinar quis lacus vel consectetur. Etiam odio risus, sagittis id cursus ac, mattis vel velit. Mauris et mauris commodo eros aliquet rhoncus non quis diam. Aenean et scelerisque velit. Suspendisse potenti. Integer malesuada metus tellus, ut lobortis arcu imperdiet vel. Pellentesque mollis rhoncus ipsum, id fringilla mi porta ultricies. Proin eget tincidunt lorem. Ut tincidunt sit amet tellus at fermentum. Ut scelerisque at massa non venenatis. Maecenas id euismod turpis. Pellentesque magna massa, malesuada sed nunc sit amet, porta auctor dui. Praesent finibus purus nisi, et faucibus dui venenatis ac. Aliquam quis posuere nulla. Sed venenatis ligula non fermentum fringilla. Etiam est tortor, ullamcorper quis massa nec, tempor aliquam purus. Nulla facilisi. Aenean accumsan tempus dignissim. Nam tincidunt fermentum lectus. Nulla fringilla tellus hendrerit, elementum dui vitae, mattis quam. Nam sed ultricies sem. Cras condimentum erat nisi, id congue ante maximus ut. Curabitur blandit in ante non accumsan. Donec elementum purus eget nunc ultricies, sed sagittis neque porttitor. Maecenas enim mauris, egestas et risus et, porttitor facilisis purus. Curabitur a dictum magna. Curabitur commodo, lacus in fermentum suscipit, arcu lacus rutrum diam, et aliquam diam mi sed orci. Suspendisse elementum consequat tellus a pretium. Sed interdum erat vitae mattis consectetur. Sed aliquam, sem ullamcorper eleifend finibus, lectus tellus convallis sapien, at fermentum ex enim nec erat. Aliquam erat volutpat. Interdum et malesuada fames ac ante ipsum primis in faucibus. In vulputate, massa in facilisis ultrices, urna mi dapibus elit, eu bibendum neque lacus et elit. Praesent vitae magna vel ligula condimentum tincidunt ac at lacus. Suspendisse at interdum justo, vitae ullamcorper sapien. Praesent nunc sem, sollicitudin et pharetra sed, laoreet ut ante. Sed elit mauris, posuere ac varius et, vulputate tincidunt odio. Suspendisse id dui ut nibh sagittis tristique. In sit amet nisi ac ipsum luctus pulvinar a in ligula. Integer porttitor lobortis felis, ac condimentum tellus maximus vel. Phasellus lobortis vulputate neque, nec faucibus quam aliquam et. Praesent pharetra arcu nec tortor imperdiet pulvinar. Nullam mattis, arcu volutpat suscipit tempor, metus mauris convallis erat, eget sagittis augue purus et est. Nam enim sapien, pretium et fermentum ac, dictum quis purus. Donec non massa blandit, auctor neque ac, vehicula lorem. In mollis ligula ullamcorper mauris iaculis venenatis. Ut condimentum, augue in vestibulum hendrerit, erat dui ultrices sem, non molestie nulla turpis sed lacus. Sed ornare rhoncus purus quis congue. Maecenas scelerisque sem eu nisi convallis, non porta tellus sodales. Aliquam ut hendrerit urna. Sed molestie lobortis sem, eget commodo mauris dapibus sed. Vivamus in magna urna. Ut elementum in arcu in gravida. Ut lacinia lacinia tempor. Ut blandit, ante non posuere commodo, lorem arcu dignissim sapien, et egestas mi urna sed ipsum. Ut quis diam eget lacus aliquet pellentesque eu ac diam. Ut blandit, libero sed eleifend congue, lectus magna suscipit arcu, quis tincidunt lorem urna sit amet lacus. Donec ornare non ex sed pellentesque. Ut in pellentesque mi. Cras faucibus lorem leo, quis maximus tellus tempor ac."
    };
    char *hashes[] = { "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9",
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
        "201e92c62b2beea4bea6cf7ab38d635b76d29fd866aadab55a4b72b3b6fc3105"
    };
    char buffer[65];
    for(int i = 0; i < NUM_PHRASES; i++){
        sha256((unsigned char *)frases[i], strlen(frases[i]), buffer);
        if( strcmp(buffer, hashes[i]) ){
            printf("erro no hash %d\n", i);
        }
    }
    return 0;
}
