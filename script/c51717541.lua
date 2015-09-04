--インフェルニティ・ブレイク
function c51717541.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetCondition(c51717541.condition)
	e1:SetTarget(c51717541.target)
	e1:SetOperation(c51717541.activate)
	c:RegisterEffect(e1)
end
function c51717541.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,LOCATION_HAND,0)==0
end
function c51717541.filter(c)
	return c:IsSetCard(0xb) and c:IsAbleToRemove()
end
function c51717541.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,nil)
		and Duel.IsExistingTarget(c51717541.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g1=Duel.SelectTarget(tp,c51717541.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	local rm=g1:GetFirst()
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,rm,1,0,0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g2=Duel.SelectTarget(tp,Card.IsDestructable,tp,0,LOCATION_ONFIELD,1,1,nil)
	local ds=g2:GetFirst()
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,ds,1,0,0)
end
function c51717541.activate(e,tp,eg,ep,ev,re,r,rp)
	local ex1,g1=Duel.GetOperationInfo(0,CATEGORY_REMOVE)
	local rm=g1:GetFirst()
	if not rm:IsRelateToEffect(e) then return end
	if Duel.Remove(rm,POS_FACEUP,REASON_EFFECT)==0 then return end
	local ex2,g2=Duel.GetOperationInfo(0,CATEGORY_DESTROY)
	local ds=g2:GetFirst()
	if not ds:IsRelateToEffect(e) then return end
	Duel.Destroy(ds,REASON_EFFECT)
end
