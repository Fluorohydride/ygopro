--地獄の番熊
function c75375465.initial_effect(c)
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_SZONE,0)
	e1:SetTarget(c75375465.indtg)
	e1:SetValue(c75375465.indval)
	c:RegisterEffect(e1)
end
function c75375465.indtg(e,c)
	return c:IsFaceup() and c:IsCode(94585852)
end
function c75375465.indval(e,re,tp)
	return e:GetHandler():GetControler()~=tp
end
