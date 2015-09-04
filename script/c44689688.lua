--ジュラック・スピノス
function c44689688.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(44689688,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c44689688.spcon)
	e1:SetTarget(c44689688.sptg)
	e1:SetOperation(c44689688.spop)
	c:RegisterEffect(e1)
end
function c44689688.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return eg:GetCount()==1 and tc:GetReasonCard()==e:GetHandler()
		and tc:IsLocation(LOCATION_GRAVE) and tc:IsReason(REASON_BATTLE) 
end
function c44689688.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
end
function c44689688.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE,tp)<=0 then return end
	if Duel.IsPlayerCanSpecialSummonMonster(tp,44689689,0,0x4011,300,0,1,RACE_DINOSAUR,ATTRIBUTE_FIRE,POS_FACEUP_ATTACK,1-tp) then
		local token=Duel.CreateToken(tp,44689689)
		Duel.SpecialSummon(token,0,tp,1-tp,false,false,POS_FACEUP_ATTACK)
	end
end
