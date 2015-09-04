--スペーシア・ギフト
function c1539051.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c1539051.target)
	e1:SetOperation(c1539051.activate)
	c:RegisterEffect(e1)
end
function c1539051.gfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x1f)
end
function c1539051.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local g=Duel.GetMatchingGroup(c1539051.gfilter,tp,LOCATION_MZONE,0,nil)
		local ct=c1539051.count_unique_code(g)
		e:SetLabel(ct)
		return ct>0 and Duel.IsPlayerCanDraw(tp,ct)
	end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,e:GetLabel())
end
function c1539051.activate(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local g=Duel.GetMatchingGroup(c1539051.gfilter,tp,LOCATION_MZONE,0,nil)
	local ct=c1539051.count_unique_code(g)
	Duel.Draw(p,ct,REASON_EFFECT)
end
function c1539051.count_unique_code(g)
	local check={}
	local count=0
	local tc=g:GetFirst()
	while tc do
		for i,code in ipairs({tc:GetCode()}) do
			if not check[code] then
				check[code]=true
				count=count+1
			end
		end
		tc=g:GetNext()
	end
	return count
end
