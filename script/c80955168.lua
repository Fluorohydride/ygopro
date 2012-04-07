--王墓の罠
function c80955168.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c80955168.condition)
	e1:SetTarget(c80955168.target)
	e1:SetOperation(c80955168.activate)
	c:RegisterEffect(e1)
end
function c80955168.cfilter(c,tp)
	return c:IsFaceup() and c:IsPreviousLocation(LOCATION_GRAVE) and c:GetPreviousControler()==1-tp
		and c:IsControler(tp) and c:IsRace(RACE_ZOMBIE)
end
function c80955168.condition(e,tp,eg,ep,ev,re,r,rp,chk)
	return eg:IsExists(c80955168.cfilter,1,nil,tp)
end
function c80955168.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,2,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,2,2,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,2,0,0)
end
function c80955168.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	Duel.Destroy(g,REASON_EFFECT)
end
