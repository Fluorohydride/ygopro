--異次元への隙間
function c49600724.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c49600724.target)
	e1:SetOperation(c49600724.activate)
	c:RegisterEffect(e1)
end
function c49600724.filter(c,e)
	return c:IsType(TYPE_MONSTER) and c:IsCanBeEffectTarget(e) and c:IsAbleToRemove()
end
function c49600724.filter1(c,g)
	return g:IsExists(Card.IsAttribute,1,c,c:GetAttribute())
end
function c49600724.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and c49600724.filter(chkc,e) end
	if chk==0 then
		local g=Duel.GetMatchingGroup(c49600724.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,nil,e)
		return g:IsExists(c49600724.filter1,1,nil,g)
	end
	local g=Duel.GetMatchingGroup(c49600724.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,nil,e)
	local rg=g:Filter(c49600724.filter1,nil,g)
	local tc=rg:GetFirst()
	local att=0
	while tc do
		att=bit.bor(att,tc:GetAttribute())
		tc=rg:GetNext()
	end
	local ac=Duel.AnnounceAttribute(tp,1,att)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local sg=rg:FilterSelect(tp,Card.IsAttribute,2,2,nil,ac)
	Duel.SetTargetCard(sg)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,sg,2,0,0)
end
function c49600724.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	Duel.Remove(sg,POS_FACEUP,REASON_EFFECT)
end
