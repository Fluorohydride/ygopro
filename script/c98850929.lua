--聖蛇の息吹
function c98850929.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(98850929,4))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1,98850929+EFFECT_COUNT_CODE_OATH)
	e1:SetTarget(c98850929.target)
	e1:SetOperation(c98850929.operation)
	c:RegisterEffect(e1)
end
function c98850929.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_FUSION+TYPE_RITUAL+TYPE_SYNCHRO+TYPE_XYZ)
end
function c98850929.typecast(c)
	return bit.band(c:GetType(),TYPE_FUSION+TYPE_RITUAL+TYPE_SYNCHRO+TYPE_XYZ)
end
function c98850929.filter1(c,e)
	return c:IsFaceup() and c:IsType(TYPE_MONSTER) and c:IsAbleToHand() and c:IsCanBeEffectTarget(e)
end
function c98850929.filter2(c,e)
	return c:IsType(TYPE_TRAP) and c:IsAbleToHand() and c:IsCanBeEffectTarget(e)
end
function c98850929.filter3(c,e)
	return c:IsType(TYPE_SPELL) and c:GetCode()~=98850929 and c:IsAbleToHand() and c:IsCanBeEffectTarget(e)
end
function c98850929.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local g=Duel.GetMatchingGroup(c98850929.cfilter,tp,LOCATION_MZONE,0,nil)
	local ct=g:GetClassCount(c98850929.typecast)
	local g1=Duel.GetMatchingGroup(c98850929.filter1,tp,LOCATION_GRAVE+LOCATION_REMOVED,0,nil,e)
	local g2=Duel.GetMatchingGroup(c98850929.filter2,tp,LOCATION_GRAVE,0,nil,e)
	local g3=Duel.GetMatchingGroup(c98850929.filter3,tp,LOCATION_GRAVE,0,nil,e)
	if chk==0 then return (ct>1 and g1:GetCount()>0) or (ct>2 and g2:GetCount()>0) or (ct>3 and g3:GetCount()>0) end
	local tg=Group.CreateGroup()
	local off=0
	repeat
		local ops={}
		local opval={}
		off=1
		if ct>1 and g1:GetCount()>0 then
			ops[off]=aux.Stringid(98850929,0)
			opval[off-1]=1
			off=off+1
		end
		if ct>2 and g2:GetCount()>0 then
			ops[off]=aux.Stringid(98850929,1)
			opval[off-1]=2
			off=off+1
		end
		if ct>3 and g3:GetCount()>0 then
			ops[off]=aux.Stringid(98850929,2)
			opval[off-1]=3
			off=off+1
		end
		local op=Duel.SelectOption(tp,table.unpack(ops))
		if opval[op]==1 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local sg=g1:Select(tp,1,1,nil)
			tg:Merge(sg)
			g1:Clear()
		elseif opval[op]==2 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local sg=g2:Select(tp,1,1,nil)
			tg:Merge(sg)
			g2:Clear()
		else
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
			local sg=g3:Select(tp,1,1,nil)
			tg:Merge(sg)
			g3:Clear()
		end
	until off<3 or not Duel.SelectYesNo(tp,aux.Stringid(98850929,3))
	Duel.SetTargetCard(tg)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,tg,tg:GetCount(),0,0)
end
function c98850929.operation(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()>0 then
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sg)
	end
end
