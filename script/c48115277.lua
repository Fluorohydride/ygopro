--ブロックマン
function c48115277.initial_effect(c)
	--check
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_PHASE_START+PHASE_DRAW)
	e1:SetCondition(c48115277.regcon)
	e1:SetOperation(c48115277.regop)
	c:RegisterEffect(e1)
	--token
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(48115277,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c48115277.spcost)
	e2:SetTarget(c48115277.sptg)
	e2:SetOperation(c48115277.spop)
	c:RegisterEffect(e2)
end
function c48115277.regcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()==tp
end
function c48115277.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ct=c:GetFlagEffectLabel(48115277)
	if not ct then
		c:RegisterFlagEffect(48115277,RESET_EVENT+0x1fe0000,0,1,0)
	else
		c:SetFlagEffectLabel(48115277,ct+1)
	end
end
function c48115277.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	local ct=e:GetHandler():GetFlagEffectLabel(48115277)
	if not ct then ct=0 end
	e:SetLabel(ct)
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c48115277.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local ct=e:GetHandler():GetFlagEffectLabel(48115277)
		if not ct then ct=0 end
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>ct-1
			and Duel.IsPlayerCanSpecialSummonMonster(tp,48115278,0,0x4011,1000,1500,4,RACE_ROCK,ATTRIBUTE_EARTH)
	end
	local ct=e:GetLabel()
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,ct-1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,ct-1,0,0)
end
function c48115277.spop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabel()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>ct
		and Duel.IsPlayerCanSpecialSummonMonster(tp,48115278,0,0x4011,1000,1500,4,RACE_ROCK,ATTRIBUTE_EARTH) then
		for i=1,ct+1 do
			local token=Duel.CreateToken(tp,48115278)
			Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			token:RegisterEffect(e1,true)
		end
		Duel.SpecialSummonComplete()
	end
end
