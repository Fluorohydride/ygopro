--ドッペルゲンガー
function c61831093.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(61831093,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c61831093.target)
	e1:SetOperation(c61831093.operation)
	c:RegisterEffect(e1)
end
function c61831093.filter(c,e)
	return c:IsFacedown() and c:IsDestructable() and c:IsCanBeEffectTarget(e)
end
function c61831093.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c61831093.filter(chkc,e) end
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(c61831093.filter,tp,LOCATION_SZONE,LOCATION_SZONE,nil,e)
	if g:GetCount()<2 then
		g:Clear()
		Duel.SetTargetCard(g)
		return
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local sg=g:Select(tp,2,2,nil)
	Duel.SetTargetCard(sg)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,sg,2,0,0)
end
function c61831093.dfilter(c,e)
	return c:IsRelateToEffect(e) and c:IsFacedown()
end
function c61831093.operation(e,tp,eg,ep,ev,re,r,rp)
	local tg=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if not tg then return end
	local dg=tg:Filter(c61831093.dfilter,nil,e)
	Duel.Destroy(dg,REASON_EFFECT)
end
