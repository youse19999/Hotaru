#include <iostream>
#include <string>
#include <Hotaru/Structure/window_context.h>
#include <window.h>

class IHotaruCommand {
public:
    virtual ~IHotaruCommand() = default;
    virtual void Run(WindowContext& context) const = 0;
};

class Document : public IHotaruCommand {
public:
    void Run(WindowContext& context) const override {

    }
};