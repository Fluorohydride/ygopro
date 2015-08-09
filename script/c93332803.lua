--Doragon, the Mad Flame Kaiju
function c93332803.initial_effect(c)
    c:SetUniqueOnField(1,0,0x223)
    --special summon
    local e1=Effect.CreateEffect(c)
    e1:SetType(EFFECT_TYPE_FIELD)
    e1:SetCode(EFFECT_SPSUMMON_PROC)
    e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
    e1:SetRange(LOCATION_HAND)
    e1:SetTargetRange(POS_FACEUP_ATTACK,1)
    e1:SetCondition(c93332803.spcon)
    e1:SetOperation(c93332803.spop)
    c:RegisterEffect(e1)
    local e2=Effect.CreateEffect(c)
    e2:SetType(EFFECT_TYPE_FIELD)
    e2:SetCode(EFFECT_SPSUMMON_PROC)
    e2:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
    e2:SetRange(LOCATION_HAND)
    e2:SetTargetRange(POS_FACEUP_ATTACK,0)
    e2:SetCondition(c93332803.spcon2)
    c:RegisterEffect(e2)
    local e3=Effect.CreateEffect(c)
    e3:SetDescription(aux.Stringid(10000000,1))
    e3:SetCategory(CATEGORY_DESTROY)
    e3:SetType(EFFECT_TYPE_IGNITION)
    e3:SetRange(LOCATION_MZONE)
    e3:SetCost(c93332803.descost)
    e3:SetTarget(c93332803.destg)
    e3:SetOperation(c93332803.desop)
    c:RegisterEffect(e3)
end
function c93332803.kaiju(e,c)
    return c:IsSetCard(0x223)
end
function c93332803.spcon(e,c)
    if c==nil then return true end
    local tp=c:GetControler()
    return Duel.GetLocationCount(1-tp,LOCATION_MZONE)>-1
        and Duel.IsExistingMatchingCard(Card.IsReleasable,tp,0,LOCATION_MZONE,1,nil)
        and not Duel.IsExistingMatchingCard(c93332803.spfilter,c:GetControler(),0,LOCATION_MZONE,1,nil)
end
function c93332803.spop(e,tp,eg,ep,ev,re,r,rp,c)
    Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
    local g=Duel.SelectMatchingCard(tp,Card.IsReleasable,tp,0,LOCATION_MZONE,1,1,nil)
    Duel.Release(g,REASON_COST)
end

function c93332803.spfilter(c)
    return c:IsFaceup() and c:IsSetCard(0x223)
end
function c93332803.spcon2(e,c)
    if c==nil then return true end
    return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0 and
        Duel.IsExistingMatchingCard(c93332803.spfilter,c:GetControler(),0,LOCATION_MZONE,1,nil)
end



function c93332803.descost(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return Duel.IsCanRemoveCounter(tp,1,1,0x2a,3,REASON_COST) end
    Duel.RemoveCounter(tp,1,1,0x2a,3,REASON_COST)
    local e1=Effect.CreateEffect(e:GetHandler())
    e1:SetType(EFFECT_TYPE_SINGLE)
    e1:SetProperty(EFFECT_FLAG_OATH)
    e1:SetCode(EFFECT_CANNOT_ATTACK)
    e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
    e:GetHandler():RegisterEffect(e1)
end
function c93332803.destg(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil) end
    local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
    Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c93332803.desop(e,tp,eg,ep,ev,re,r,rp)
    local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
    Duel.Destroy(g,REASON_EFFECT)
end
