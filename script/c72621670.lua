--ダブルマジックアームバインド
function c72621670.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_CONTROL)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCost(c72621670.cost)
	e1:SetTarget(c72621670.target)
	e1:SetOperation(c72621670.activate)
	c:RegisterEffect(e1)
end
function c72621670.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,nil,2,nil) end
	local g=Duel.SelectReleaseGroup(tp,nil,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c72621670.filter(c)
	return c:IsFaceup() and c:IsAbleToChangeControler()
end
function c72621670.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE,1-tp,LOCATION_REASON_CONTROL)>=0
		and Duel.IsExistingTarget(c72621670.filter,tp,0,LOCATION_MZONE,2,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
	local g=Duel.SelectTarget(tp,c72621670.filter,tp,0,LOCATION_MZONE,2,2,nil)
	Duel.SetOperationInfo(0,CATEGORY_CONTROL,g,2,0,0)
end
function c72621670.tfilter(c,e)
	return c:IsRelateToEffect(e) and c:IsFaceup()
end
function c72621670.tfilter2(c,e)
	return not c:IsImmuneToEffect(e) and c:IsAbleToChangeControler()
end
function c72621670.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(c72621670.tfilter,nil,e)
	if g:GetCount()<2 then return end
	local ct=Duel.GetLocationCount(tp,LOCATION_MZONE,1-tp,LOCATION_REASON_CONTROL)
	local g1=g:Filter(c72621670.tfilter2,nil,e)
	local sg=nil
	local dg=g1:Clone()
	if g1:GetCount()>ct then
		if ct>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONTROL)
			sg=g1:Select(tp,1,1,nil)
		else
			sg=Group.CreateGroup()
		end
		dg:Sub(sg)
	else
		sg=g1:Clone()
		dg:Clear()
	end
	local tc=sg:GetFirst()
	while tc do
		Duel.GetControl(tc,tp,PHASE_END+RESET_SELF_TURN,1)
		tc=sg:GetNext()
	end
	if dg:GetCount()>0 then Duel.Destroy(dg,REASON_RULE) end
end
