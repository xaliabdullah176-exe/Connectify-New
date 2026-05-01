#include "user.h"

void User::createPost(Post* p) {
    resize((User**&)posts, postCount);  // reuse resize pattern
    posts[postCount++] = p;
}

void User::showPosts() {
    if (postCount == 0) { cout << "No posts yet." << endl; return; }
    for (int i = 0; i < postCount; i++)
        posts[i]->display();
}

void User::showNewsFeed() {
    int total = 0;

    // STEP 1: count posts from followed users
    for (int i = 0; i < followingCount; i++) {
        total += following[i]->postCount;
    }

    // include your own posts
    total += postCount;

    // if no posts
    if (total == 0) {
        cout << "No posts available\n";
        return;
    }

    //: allocate exact memory
    Post** feed = new Post * [total];
    User** owner = new User * [total];

    int idx = 0;

    //  collect posts from followed users
    for (int i = 0; i < followingCount; i++) {
        User* u = following[i];

        for (int j = 0; j < u->postCount; j++) {
            feed[idx] = u->posts[j];
            owner[idx] = u;
            idx++;
        }
    }

    // include own posts
    for (int i = 0; i < postCount; i++) {
        feed[idx] = posts[i];
        owner[idx] = this;
        idx++;
    }

    //  sort by timestamp (latest first)
    for (int i = 0; i < total - 1; i++) {
        for (int j = i + 1; j < total; j++) {
            if (feed[i]->timestamp < feed[j]->timestamp) {
                Post* ptemp = feed[i];
                feed[i] = feed[j];
                feed[j] = ptemp;
                User* utemp = owner[i];
                owner[i] = owner[j];
                owner[j] = utemp;
            }
        }
    }

    
    cout << "\n========== NEWS FEED ==========\n";

    for (int i = 0; i < total; i++) {
        cout << "User: " << owner[i]->userName << endl;
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &feed[i]->timestamp);
        cout << "Time: " << buffer;
        cout << "Content: " << feed[i]->content << endl;
        cout << "Likes: " << feed[i]->likeCount << endl;

        for (int j = 0; j < feed[i]->commentCount; j++) {
            cout << " - " << feed[i]->comments[j] << endl;
        }

        cout << "-----------------------------\n";
    }

   
    delete[] feed;
    delete[] owner;
}

