--棘の妖精
function c91559748.initial_effect(c)
	--change position
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(91559748,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DAMAGE_STEP_END)
	e1:SetCondition(c91559748.cpcon)
	e1:SetTarget(c91559748.cptg)
	e1:SetOperation(c91559748.cpop)
	c:RegisterEffect(e1)
	--cannot be battle target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(0,LOCATION_MZONE)
	e2:SetValue(c91559748.tg)
	c:RegisterEffect(e2)
end
function c91559748.tg(e,c)
	return c:IsFaceup() and c:IsRace(RACE_INSECT)
end
function c91559748.cpcon(e,tp,eg,ep,ev,re,r,rp)
	local t=e:GetHandler():GetBattleTarget()
	e:SetLabelObject(t)
	return t and t:IsRelateToBattle()
end
function c91559748.cptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_POSITION,e:GetLabelObject(),1,0,0)
end
function c91559748.cpop(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject()
	if g:IsRelateToBattle() and g:IsAttackPos() then
		Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
	end
end
