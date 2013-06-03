--カラクリ忍者 九壱九
function c6276588.initial_effect(c)
	--must attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_MUST_ATTACK)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_EP)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,0)
	e2:SetCondition(c6276588.becon)
	c:RegisterEffect(e2)
	--pos change
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(6276588,0))
	e3:SetCategory(CATEGORY_POSITION)
	e3:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e3:SetCode(EVENT_BE_BATTLE_TARGET)
	e3:SetOperation(c6276588.posop)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(6276588,1))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetCode(EVENT_BATTLE_DESTROYED)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCondition(c6276588.spcon)
	e4:SetTarget(c6276588.sptg)
	e4:SetOperation(c6276588.spop)
	c:RegisterEffect(e4)
end
function c6276588.becon(e)
	return e:GetHandler():IsAttackable()
end
function c6276588.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		Duel.ChangePosition(c,POS_FACEUP_DEFENCE,0,POS_FACEUP_ATTACK,0)
	end
end
function c6276588.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return eg:GetCount()==1 and tc:GetReasonCard()==e:GetHandler()
		and tc:IsLocation(LOCATION_GRAVE) and tc:IsReason(REASON_BATTLE) 
end
function c6276588.filter(c,e,tp)
	return c:IsLevelBelow(4) and c:IsSetCard(0x11) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c6276588.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c6276588.filter(chkc,e,tp) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c6276588.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,g:GetCount(),0,0)
end
function c6276588.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
	end
end
