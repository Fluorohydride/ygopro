--サボウ・ファイター
function c74440055.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(74440055,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(c74440055.condition)
	e1:SetTarget(c74440055.target)
	e1:SetOperation(c74440055.operation)
	c:RegisterEffect(e1)
end
function c74440055.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsRelateToBattle() and c:GetBattleTarget():IsType(TYPE_MONSTER)
end
function c74440055.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,tp,0)
end
function c74440055.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(1-tp,LOCATION_MZONE)>0
		and Duel.IsPlayerCanSpecialSummonMonster(tp,74440056,0,0x4011,500,500,1,RACE_PLANT,ATTRIBUTE_EARTH,POS_FACEUP_DEFENCE,1-tp) then
		local token=Duel.CreateToken(tp,74440056)
		Duel.SpecialSummon(token,0,tp,1-tp,false,false,POS_FACEUP_DEFENCE)
	end
end
