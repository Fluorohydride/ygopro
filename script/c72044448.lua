--破滅のフォトン・ストリーム
function c72044448.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c72044448.condition)
	e1:SetTarget(c72044448.target)
	e1:SetOperation(c72044448.activate)
	c:RegisterEffect(e1)
end
function c72044448.cfilter1(c)
	return c:IsFaceup() and c:IsSetCard(0x107b)
end
function c72044448.cfilter2(c)
	return c:IsFaceup() and c:IsCode(93717133)
end
function c72044448.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c72044448.cfilter1,tp,LOCATION_MZONE,0,1,nil)
		and (Duel.GetTurnPlayer()==tp or Duel.IsExistingMatchingCard(c72044448.cfilter2,tp,LOCATION_ONFIELD,0,1,nil))
end
function c72044448.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsAbleToRemove() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,Card.IsAbleToRemove,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c72044448.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
