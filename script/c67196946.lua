--スター・ブラスト
function c67196946.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c67196946.con)
	e1:SetCost(c67196946.cost)
	e1:SetTarget(c67196946.tg)
	e1:SetOperation(c67196946.op)
	c:RegisterEffect(e1)
end
function c67196946.con(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsPlayerAffectedByEffect(tp,EFFECT_LPCOST_CHANGE)
end
function c67196946.filter(c,lv)
	return c:GetLevel()>lv
end
function c67196946.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,500) end
	local lp=Duel.GetLP(tp)
	local g=Duel.GetMatchingGroup(c67196946.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,nil,1)
	local tg=g:GetMaxGroup(Card.GetLevel)
	local maxlv=tg:GetFirst():GetLevel()
	local t={}
	local l=1
	while l<maxlv and l*500<lp do
		t[l]=l*500
		l=l+1
	end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(67196946,0))
	local announce=Duel.AnnounceNumber(tp,table.unpack(t))
	Duel.PayLPCost(tp,announce)
	e:SetLabel(announce/500)
end
function c67196946.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c67196946.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,1,nil,1) end
end
function c67196946.op(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	local g=Duel.GetMatchingGroup(c67196946.filter,tp,LOCATION_HAND+LOCATION_MZONE,0,nil,ct)
	if g:GetCount()>0 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(67196946,1))
		local sg=g:Select(tp,1,1,nil)
		local tc=sg:GetFirst()
		if tc:IsLocation(LOCATION_MZONE) then
			Duel.HintSelection(sg)
		end
		Duel.ConfirmCards(1-tp,sg)
		if tc:IsLocation(LOCATION_HAND) then Duel.ShuffleHand(tp) end
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetReset(RESET_EVENT+0xfe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(-ct)
		tc:RegisterEffect(e1)
	end
end
