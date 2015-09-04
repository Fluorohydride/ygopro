--精気を吸う骨の塔
function c63012333.initial_effect(c)
	--cannot be target
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c63012333.atklm)
	e1:SetValue(aux.imval1)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(63012333,0))
	e2:SetCategory(CATEGORY_DECKDES)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c63012333.condition)
	e2:SetTarget(c63012333.target)
	e2:SetOperation(c63012333.operation)
	c:RegisterEffect(e2)
end
function c63012333.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_ZOMBIE)
end
function c63012333.atklm(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c63012333.filter,c:GetControler(),LOCATION_MZONE,0,1,c)
end
function c63012333.condition(e,tp,eg,ep,ev,re,r,rp)
	return not eg:IsContains(e:GetHandler()) and eg:IsExists(c63012333.filter,1,nil)
end
function c63012333.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,1-tp,2)
end
function c63012333.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(1-tp,2,REASON_EFFECT)
end
