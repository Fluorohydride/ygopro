--オプション
function c14291024.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c14291024.spcon)
	c:RegisterEffect(e1)
	--spsummon con
	local e2=Effect.CreateEffect(c)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SPSUMMON_CONDITION)
	e2:SetValue(c14291024.splimit)
	c:RegisterEffect(e2)
	--set target
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetOperation(c14291024.tgop)
	c:RegisterEffect(e3)
	--atk.def
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_SET_ATTACK_FINAL)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCondition(c14291024.adcon)
	e4:SetValue(c14291024.atkval)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_SET_DEFENCE_FINAL)
	e5:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCondition(c14291024.adcon)
	e5:SetValue(c14291024.defval)
	c:RegisterEffect(e5)
	--destroy
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCondition(c14291024.sdcon)
	c:RegisterEffect(e6)
end
function c14291024.filter(c)
	return c:IsFaceup() and c:IsCode(10992251)
end
function c14291024.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and	Duel.IsExistingMatchingCard(c14291024.filter,c:GetControler(),LOCATION_MZONE,0,1,nil)
end
function c14291024.splimit(e,se,sp,st,pos,top)
	if bit.band(pos,POS_FACEDOWN)~=0 then return false end
	return Duel.IsExistingMatchingCard(c14291024.filter,top,LOCATION_MZONE,0,1,nil)
end
function c14291024.tgop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(14291024,0))
	local g=Duel.SelectMatchingCard(tp,c14291024.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.HintSelection(g)
	c:SetCardTarget(g:GetFirst())
	c:RegisterFlagEffect(14291024,RESET_EVENT+0x1ff0000,0,1)
end
function c14291024.adcon(e)
	return e:GetHandler():GetFirstCardTarget()~=nil
end
function c14291024.atkval(e,c)
	return c:GetFirstCardTarget():GetAttack()
end
function c14291024.defval(e,c)
	return c:GetFirstCardTarget():GetDefence()
end
function c14291024.sdcon(e)
	return e:GetHandler():GetFirstCardTarget()==nil and e:GetHandler():GetFlagEffect(14291024)~=0
end
