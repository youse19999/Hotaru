/**
 * @class HotaruGPU
 * @brief WebGPUのラッパー
 * @author Saitou Yousei
 */
class HotaruGPU
{
public:
	void CreateGPU()
	{

	}
};
struct HotaruEnt {
	virtual ~HotaruEnt() = default;
	virtual void print() const = 0;
};
/*
Game().Select("MainCamera").GenMat4FromLookAt();
*/