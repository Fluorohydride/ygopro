--悪魔の憑代
function c72497366.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--decrease tribute
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(72497366,0))
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SUMMON_PROC)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_HAND,0)
	e2:SetCountLimit(1)
	e2:SetCondition(c72497366.ntcon)
	e2:SetTarget(c72497366.nttg)
	c:RegisterEffect(e2)
	--destroy replace
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DESTROY_REPLACE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTarget(c72497366.reptg)
	e3:SetValue(1)
	c:RegisterEffect(e3)
end
function c72497366.ntcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
end
function c72497366.nttg(e,c)
	return c:IsLevelAbove(5) and c:IsRace(RACE_FIEND)
end
function c72497366.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=eg:GetFirst()
	if chk==0 then return eg:GetCount()==1 and tc:IsFaceup() and tc:IsLocation(LOCATION_MZONE)
		and tc:IsRace(RACE_FIEND) and tc:IsLevelAbove(5) and bit.band(tc:GetSummonType(),SUMMON_TYPE_NORMAL)~=0 end
	if Duel.SelectYesNo(tp,aux.Stringid(72497366,1)) then
		Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
		return true
	else return false end
end
