#import "Utils.h"
#import <AppKit/NSAlert.h>
#import <MetalKit/MetalKit.h>

void ShowDialog(const char *message, const char *info) {
    NSAlert *alert = [[NSAlert alloc] init];
    alert.alertStyle = NSAlertStyleInformational;
    alert.messageText = @(message);
    alert.informativeText = @(info);
    [alert runModal];
}

MTL::Texture *LoadTextureFromImagePath(MTL::Device *pDevice, const char *filePath) {
    MTKTextureLoader *loader = [[MTKTextureLoader alloc] initWithDevice:(__bridge id<MTLDevice>)pDevice];
    NSURL *baseURL = [NSURL fileURLWithPath:@"file://"];
    NSURL *URL = [NSURL URLWithString:@(filePath) relativeToURL:baseURL];
    NSError *__autoreleasing err = nil;
    NSDictionary<MTKTextureLoaderOption, id> *options = @{
        MTKTextureLoaderOptionTextureStorageMode : @(MTLStorageModeShared),
        MTKTextureLoaderOptionTextureUsage : @(MTLTextureUsageShaderRead),
        MTKTextureLoaderOptionSRGB : @(false),
    };
    id<MTLTexture> texture = [loader newTextureWithContentsOfURL:URL options:options error:&err];
    return (__bridge_retained MTL::Texture *)texture;
}