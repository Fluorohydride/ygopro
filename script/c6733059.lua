--死霊の巣
function c6733059.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1c0)
	e1:SetCost(c6733059.cost1)
	e1:SetOperation(c6733059.operation)
	c:RegisterEffect(e1)
	--instant(chain)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(6733059,1))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,0x1c0)
	e2:SetCost(c6733059.cost2)
	e2:SetOperation(c6733059.operation)
	c:RegisterEffect(e2)
end
function c6733059.cfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToRemoveAsCost()
end
function c6733059.tfilter(c,lv)
	return c:IsFaceup() and c:IsLevelBelow(lv) and c:IsDestructable()
end
function c6733059.cost1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local cg=Duel.GetMatchingGroup(c6733059.cfilter,tp,LOCATION_GRAVE,0,nil)
	local tg=Duel.GetMatchingGroup(c6733059.tfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,cg:GetCount())
	if tg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(6733059,0)) then
		local lvt={}
		local tc=tg:GetFirst()
		while tc do
			local tlv=tc:GetLevel()
			lvt[tlv]=tlv
			tc=tg:GetNext()
		end
		local pc=1
		for i=1,12 do
			if lvt[i] then lvt[i]=nil lvt[pc]=i pc=pc+1 end
		end
		lvt[pc]=nil
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(6733059,2))
		local lv=Duel.AnnounceNumber(tp,table.unpack(lvt))
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
		local rg=cg:Select(tp,lv,lv,nil)
		Duel.Remove(rg,POS_FACEUP,REASON_COST)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,tg,1,0,0)
		Duel.SetTargetParam(lv)
	end
end
function c6733059.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local cg=Duel.GetMatchingGroup(c6733059.cfilter,tp,LOCATION_GRAVE,0,nil)
		local tg=Duel.GetMatchingGroup(c6733059.tfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,cg:GetCount())
		return tg:GetCount()>0
	end
	e:SetLabel(0)
	local cg=Duel.GetMatchingGroup(c6733059.cfilter,tp,LOCATION_GRAVE,0,nil)
	local tg=Duel.GetMatchingGroup(c6733059.tfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,cg:GetCount())
	local lvt={}
	local tc=tg:GetFirst()
	while tc do
		local tlv=tc:GetLevel()
		lvt[tlv]=tlv
		tc=tg:GetNext()
	end
	local pc=1
	for i=1,12 do
		if lvt[i] then lvt[i]=nil lvt[pc]=i pc=pc+1 end
	end
	lvt[pc]=nil
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(6733059,2))
	local lv=Duel.AnnounceNumber(tp,table.unpack(lvt))
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rg=cg:Select(tp,lv,lv,nil)
	Duel.Remove(rg,POS_FACEUP,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tg,1,0,0)
	Duel.SetTargetParam(lv)
end
function c6733059.dfilter(c,lv)
	return c:IsFaceup() and c:GetLevel()==lv and c:IsDestructable()
end
function c6733059.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local lv=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	if lv==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectMatchingCard(tp,c6733059.dfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,lv)
	Duel.Destroy(g,REASON_EFFECT)
end
