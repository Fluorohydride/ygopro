--毒蛇の供物
function c93217231.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e0)
	e1:SetTarget(c93217231.target)
	e1:SetOperation(c93217231.activate)
	c:RegisterEffect(e1)
end
function c93217231.filter1(c)
	return c:IsFaceup() and c:IsRace(RACE_REPTILE) and c:IsDestructable()
end
function c93217231.filter2(c)
	return c:IsDestructable()
end
function c93217231.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.IsExistingTarget(c93217231.filter1,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(c93217231.filter2,tp,0,LOCATION_ONFIELD,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g1=Duel.SelectTarget(tp,c93217231.filter1,tp,LOCATION_MZONE,0,1,1,nil)
	e:SetLabelObject(g1:GetFirst())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g2=Duel.SelectTarget(tp,c93217231.filter2,tp,0,LOCATION_ONFIELD,2,2,nil)
	g1:Merge(g2)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g1,g1:GetCount(),0,0)
end
function c93217231.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	local sc=e:GetLabelObject()
	if sg:GetCount()~=3 or sc:IsFacedown() or not sc:IsRace(RACE_REPTILE) or sc:IsControler(1-tp) then return end
	Duel.Destroy(sg,REASON_EFFECT)
end
