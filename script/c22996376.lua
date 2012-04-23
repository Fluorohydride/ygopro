--百獣王 ベヒーモス
function c22996376.initial_effect(c)
	--summon/set with 1 tribute
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(22996376,0))
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SUMMON_PROC)
	e1:SetCondition(c22996376.otcon)
	e1:SetOperation(c22996376.otop)
	e1:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_SET_PROC)
	c:RegisterEffect(e2)
	--salvage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(22996376,1))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetCondition(c22996376.thcon)
	e3:SetTarget(c22996376.thtg)
	e3:SetOperation(c22996376.thop)
	c:RegisterEffect(e3)
end
function c22996376.otcon(e,c)
	if c==nil then return true end
	return c:GetLevel()>6 and Duel.GetTributeCount(c)>0
end
function c22996376.otop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.SelectTribute(tp,c,1,1)
	c:SetMaterial(g)
	Duel.Release(g,REASON_SUMMON+REASON_MATERIAL)
	--change base attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetReset(RESET_EVENT+0xff0000)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetValue(2000)
	c:RegisterEffect(e1)
end
function c22996376.thcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c22996376.filter(c)
	return c:IsRace(RACE_BEAST) and c:IsAbleToHand()
end
function c22996376.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c22996376.filter(chkc) end
	local ct=e:GetHandler():GetMaterialCount()
	if chk==0 then return ct>0 and Duel.IsExistingTarget(c22996376.filter,tp,LOCATION_GRAVE,0,ct,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c22996376.filter,tp,LOCATION_GRAVE,0,ct,ct,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,ct,0,0)
end
function c22996376.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	Duel.SendtoHand(sg,nil,REASON_EFFECT)
end
