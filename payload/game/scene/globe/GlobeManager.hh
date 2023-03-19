namespace Scene {

class GlobeManager {
public:
    void setLocation(u32 r4, f32 f1, f32 f2);
    void thunk_34_14();

    static GlobeManager *Instance() {
        return s_instance;
    };

private:
    u8 _00[0x3f - 0x0];

    static GlobeManager *s_instance;
};

static_assert(sizeof(GlobeManager) == 0x3f);

} // namespace Scene
