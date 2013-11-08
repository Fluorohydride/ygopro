--増殖
function c40703222.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c40703222.cost)
	e1:SetTarget(c40703222.target)
	e1:SetOperation(c40703222.activate)
	c:RegisterEffect(e1)
end
function c40703222.cfilter(c)
	return c:IsFaceup() and c:IsCode(40640057)
end
function c40703222.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,c40703222.cfilter,1,nil) end
	local g=Duel.SelectReleaseGroup(tp,c40703222.cfilter,1,1,nil)
	Duel.Release(g,REASON_COST)
end
function c40703222.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsPlayerCanSpecialSummonMonster(tp,40703223,0,0x4011,300,200,1,RACE_FIEND,ATTRIBUTE_DARK) end
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,ft,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,ft,0,0)
end
function c40703222.activate(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=0 or not Duel.IsPlayerCanSpecialSummonMonster(tp,40703223,0,0x4011,300,200,1,RACE_FIEND,ATTRIBUTE_DARK) then return end
	for i=1,ft do
		local token=Duel.CreateToken(tp,40703223)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UNRELEASABLE_SUM)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(1)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		token:RegisterEffect(e1,true)
	end
	Duel.SpecialSummonComplete()
end
