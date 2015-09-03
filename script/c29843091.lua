--おジャマトリオ
function c29843091.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetTarget(c29843091.target)
	e1:SetOperation(c29843091.activate)
	c:RegisterEffect(e1)
end
function c29843091.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(1-tp,LOCATION_MZONE,tp)>2
		and Duel.IsPlayerCanSpecialSummonMonster(tp,29843092,0xf,0x4011,0,1000,2,RACE_BEAST,ATTRIBUTE_LIGHT,POS_FACEUP_DEFENCE,1-tp) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,3,0,0)
end
function c29843091.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE,tp)<3 then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,29843092,0xf,0x4011,0,1000,2,RACE_BEAST,ATTRIBUTE_LIGHT,POS_FACEUP_DEFENCE,1-tp) then return end
	local g=Group.CreateGroup()
	for i=1,3 do
		local token=Duel.CreateToken(tp,29843091+i)
		if Duel.SpecialSummonStep(token,0,tp,1-tp,false,false,POS_FACEUP_DEFENCE) then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UNRELEASABLE_SUM)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			e1:SetValue(1)
			token:RegisterEffect(e1,true)
			g:AddCard(token)
		end
	end
	g:KeepAlive()
	local e2=Effect.CreateEffect(e:GetHandler())
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_DESTROYED)
	e2:SetLabelObject(g)
	e2:SetCondition(c29843091.damcon)
	e2:SetOperation(c29843091.damop)
	Duel.RegisterEffect(e2,tp)
	Duel.SpecialSummonComplete()
end
function c29843091.dfilter(c,g)
	return g:IsContains(c)
end
function c29843091.damcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if eg:IsExists(c29843091.dfilter,1,nil,g) then
		return true
	else
		if not g:IsExists(Card.IsLocation,1,nil,LOCATION_MZONE) then
			g:DeleteGroup()
			e:Reset()
		end
		return false
	end
end
function c29843091.damop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	local tc=eg:GetFirst()
	while tc do
		if g:IsContains(tc) then
			Duel.Damage(tc:GetPreviousControler(),300,REASON_EFFECT)
			g:RemoveCard(tc)
		end
		tc=eg:GetNext()
	end
end
