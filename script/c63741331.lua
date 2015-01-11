--フォッグ・コントロール
function c63741331.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c63741331.cost)
	e1:SetTarget(c63741331.target)
	e1:SetOperation(c63741331.activate)
	c:RegisterEffect(e1)
end
function c63741331.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x18)
end
function c63741331.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c63741331.cfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c63741331.cfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c63741331.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c63741331.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		tc:AddCounter(0x19,3)
	end
end
