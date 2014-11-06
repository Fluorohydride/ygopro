--U.A. Midfielder
function c72491806.initial_effect(c)
    --special summon
    local e1=Effect.CreateEffect(c)
    e1:SetType(EFFECT_TYPE_FIELD)
    e1:SetCode(EFFECT_SPSUMMON_PROC)
    e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
    e1:SetRange(LOCATION_HAND)
    e1:SetCountLimit(1,72491806)
    e1:SetCondition(c72491806.spcon)
    e1:SetOperation(c72491806.spop)
    c:RegisterEffect(e1)
    --to hand and spsummon
    local e2=Effect.CreateEffect(c)
    e2:SetDescription(aux.Stringid(72491806,0))
    e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SPECIAL_SUMMON)
    e2:SetType(EFFECT_TYPE_QUICK_O)
    e2:SetCode(EVENT_FREE_CHAIN)
    e2:SetRange(LOCATION_MZONE)
    e2:SetCountLimit(1,172491806)
    e2:SetTarget(c72491806.tstg)
    e2:SetOperation(c72491806.tsop)
    c:RegisterEffect(e2)
end
function c72491806.spfilter(c)
    return c:IsFaceup() and c:IsSetCard(0xb2) and not c:IsCode(72491806) and c:IsAbleToHandAsCost()
end
function c72491806.spcon(e,c)
    if c==nil then return true end
    return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-1
        and Duel.IsExistingMatchingCard(c72491806.spfilter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c72491806.spop(e,tp,eg,ep,ev,re,r,rp,c)
    Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
    local g=Duel.SelectMatchingCard(tp,c72491806.spfilter,tp,LOCATION_MZONE,0,1,1,nil)
    Duel.SendtoHand(g,nil,REASON_COST)
end
function c72491806.thfilter(c,e)
    return c:IsSetCard(0xb2) and c:IsAbleToHand() and c:IsCanBeEffectTarget(e)
end
function c72491806.thfilter2(c,code)
    return not c:IsCode(code)
end
function c72491806.spfilter2(c,e,tp,g)
    return c:IsSetCard(0xb2) and c:IsCanBeSpecialSummoned(e,0,tp,false,false) 
      and g:IsExists(c72491806.thfilter2,1,e:GetHandler(),c:GetCode())
end
function c72491806.tstg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
    local g=Duel.GetMatchingGroup(c72491806.thfilter,tp,LOCATION_MZONE,0,nil,e)
    if chkc then return chkc:IsFaceup() and chkc:IsAbleToHand() end
    if chk==0 then return g:GetCount()>0 and Duel.IsExistingMatchingCard(c72491806.spfilter2,tp,LOCATION_HAND,0,1,nil,e,tp,g) and Duel.GetLocationCount(tp,LOCATION_MZONE)>-1 end
    Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
    local tg=Duel.SelectTarget(tp,c72491806.thfilter,tp,LOCATION_MZONE,0,1,1,e:GetHandler(),e)
    Duel.SetOperationInfo(0,CATEGORY_TOHAND,tg,tg:GetCount(),0,0)
    Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c72491806.tsop(e,tp,eg,ep,ev,re,r,rp)
    local tc=Duel.GetFirstTarget()
    if tc:IsRelateToEffect(e) then
        Duel.SendtoHand(tc,nil,REASON_EFFECT)
    end
    local og=Duel.GetOperatedGroup()
    if og:GetCount()>0 and Duel.IsExistingMatchingCard(c72491806.spfilter2,tp,LOCATION_HAND,0,1,nil,e,tp,og) and Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
        Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
        sg=Duel.SelectMatchingCard(tp,c72491806.spfilter2,tp,LOCATION_HAND,0,1,1,nil,e,tp,og)
        if sg:GetCount()>0 then
            Duel.SpecialSummon(sg,0,tp,tp,false,false,POS_FACEUP)
        end
    end
end
        