--オイスターマイスター
function c83239739.initial_effect(c)
	--token
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(83239739,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c83239739.condition)
	e1:SetTarget(c83239739.target)
	e1:SetOperation(c83239739.operation)
	c:RegisterEffect(e1)
end
function c83239739.condition(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetPreviousLocation(),LOCATION_ONFIELD)>0
		and not (bit.band(r,REASON_BATTLE+REASON_DESTROY)==REASON_BATTLE+REASON_DESTROY)
end
function c83239739.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
end
function c83239739.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<1 then return end
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,83239740,0,0x4011,0,0,1,RACE_FISH,ATTRIBUTE_WATER) then return end
	local token=Duel.CreateToken(tp,83239740)
	Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
end
