--大金星！？
function c84677654.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c84677654.target)
	e1:SetOperation(c84677654.operation)
	c:RegisterEffect(e1)
end
function c84677654.cfilter(c)
	return c:IsFaceup() and not c:IsType(TYPE_XYZ)
end
function c84677654.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c84677654.cfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,567)
	local lv=Duel.AnnounceNumber(tp,1,2,3,4,5,6,7,8,9,10,11,12)
	Duel.SetTargetParam(lv)
	Duel.SetOperationInfo(0,CATEGORY_COIN,nil,0,PLAYER_ALL,2)
end
function c84677654.operation(e,tp,eg,ep,ev,re,r,rp)
	local lv=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	local coin1=Duel.TossCoin(tp,1)
	local coin2=Duel.TossCoin(1-tp,1)
	if coin1==1 and coin2==1 then
		local g=Duel.GetMatchingGroup(c84677654.cfilter,tp,LOCATION_MZONE,0,nil)
		local tc=g:GetFirst()
		while tc do
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CHANGE_LEVEL)
			e1:SetValue(lv)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			tc:RegisterEffect(e1)
			tc=g:GetNext()
		end
	elseif coin1==0 and coin2==0 then
		local lp=Duel.GetLP(tp)-lv*500
		Duel.SetLP(tp,lp>0 and lp or 0)
	end
end
